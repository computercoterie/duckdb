#include "planner/expression_binder/order_binder.hpp"

#include "parser/expression/bound_columnref_expression.hpp"
#include "parser/expression/columnref_expression.hpp"
#include "parser/expression/constant_expression.hpp"
#include "parser/query_node/select_node.hpp"

using namespace duckdb;
using namespace std;

OrderBinder::OrderBinder(Binder &binder, ClientContext &context, SelectNode &node,
                         unordered_map<string, uint32_t> &alias_map, expression_map_t<uint32_t> &projection_map)
    : SelectNodeBinder(binder, context, node), alias_map(alias_map), projection_map(projection_map) {
}

BindResult OrderBinder::CreateProjectionReference(Expression &expr, size_t index) {
	return BindResult(make_unique<BoundColumnRefExpression>(expr, TypeId::INVALID,
	                                                        ColumnBinding(node.binding.projection_index, index), 0));
}

BindResult OrderBinder::BindExpression(unique_ptr<Expression> expr, uint32_t depth, bool root_expression) {
	// in the ORDER BY clause we do not bind children
	// we bind ONLY to the select list
	// if there is no matching entry in the SELECT list already, we add the expression to the SELECT list and refer the
	// new expression the new entry will then be bound later during the binding of the SELECT list we also don't do type
	// resolution here: this only happens after the SELECT list has been bound
	switch (expr->GetExpressionClass()) {
	case ExpressionClass::CONSTANT: {
		// ORDER BY constant
		// is the ORDER BY expression a constant integer? (e.g. ORDER BY 1)
		auto &constant = (ConstantExpression &)*expr;
		// ORDER BY a constant
		if (!TypeIsIntegral(constant.value.type)) {
			// non-integral expression, we just leave the constant here.
			// ORDER BY <constant> has no effect
			// CONTROVERSIAL: maybe we should throw an error
			return BindResult(nullptr);
		}
		// INTEGER constant: we use the integer as an index into the select list (e.g. ORDER BY 1)
		auto index = constant.value.GetNumericValue();
		if (index < 1 || index > node.select_list.size()) {
			throw BinderException("ORDER term out of range - should be between 1 and %d", (int)node.select_list.size());
		}
		return CreateProjectionReference(*expr, index - 1);
	}
	case ExpressionClass::COLUMN_REF: {
		// COLUMN REF expression
		// check if we can bind it to an alias in the select list
		auto &colref = (ColumnRefExpression &)*expr;
		// if there is an explicit table name we can't bind to an alias
		if (!colref.table_name.empty()) {
			break;
		}
		// check the alias list
		auto entry = alias_map.find(colref.column_name);
		if (entry != alias_map.end()) {
			// it does! point it to that entry
			return CreateProjectionReference(*expr, entry->second);
		}
	}
	default:
		break;
	}
	// general case
	// first check if the ORDER BY clause already points to an entry in the projection list
	auto entry = projection_map.find(expr.get());
	if (entry != projection_map.end()) {
		// there is a matching entry in the projection list
		// just point to that entry
		return CreateProjectionReference(*expr, entry->second);
	}
	// otherwise we need to push the ORDER BY entry into the select list
	auto result = CreateProjectionReference(*expr, node.select_list.size());
	node.select_list.push_back(move(expr));
	return result;
}
