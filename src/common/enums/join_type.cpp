#include "duckdb/common/enums/join_type.hpp"
#include "duckdb/common/enum_util.hpp"

namespace duckdb {

bool IsLeftOuterJoin(JoinType type) {
	return type == JoinType::LEFT || type == JoinType::OUTER;
}

bool IsRightOuterJoin(JoinType type) {
	return type == JoinType::OUTER || type == JoinType::RIGHT;
}

} // namespace duckdb
