//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/catalog/default/default_functions.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/catalog/default/default_generator.hpp"

namespace duckdb {
class SchemaCatalogEntry;
struct CreateMacroInfo;

struct DefaultMacro {
	const char *schema;
	const char *name;
	const char *parameters[8];
	const char *macro;
};

class DefaultFunctionGenerator : public DefaultGenerator {
public:
	DefaultFunctionGenerator(Catalog &catalog, SchemaCatalogEntry *schema);

	SchemaCatalogEntry *schema;

	static unique_ptr<CreateMacroInfo> CreateInternalMacroInfo(DefaultMacro &default_macro);

public:
	unique_ptr<CatalogEntry> CreateDefaultEntry(ClientContext &context, const string &entry_name) override;
	vector<string> GetDefaultEntries() override;
};

} // namespace duckdb
