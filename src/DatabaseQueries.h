#pragma once
#include "Resource.h"

const QString& MakeResourcesTableCreateQuery();
QString MakeResourceSelectQuery(int rowId);
QString MakeResourceSelectPropertyQuery(int rowId, ResourceProperty prop);
const QString& MakeResourceSelectDefinitionsQuery();
QString MakeResourceUpdateQuery(int rowId);
QString MakeResourcePropertyUpdateQuery(int rowId, ResourceProperty property);
QString MakeResourceInsertQuery();
QString MakeResourceInsertQueryValues();
