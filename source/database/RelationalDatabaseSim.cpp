#include "util.h"
#include "datatypes.h"
#include "RelationalDatabaseSim.h"


DbQuerySim&
DbconnClassSim::query() {
  _queries.push_back(DbQuerySim(this));
  return _queries[_queries.size() - 1];
}

DbconnClassSim::~DbconnClassSim() {
  for (auto q: _queries) {
    if (q.ptr()) delete q.ptr();
  }
}
