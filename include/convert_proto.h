#ifndef NBA_QUERIES_CONVERT_PROTO_H
#define NBA_QUERIES_CONVERT_PROTO_H
#include <vector>
#include "moment.capnp.h"
#include "query_structs.h"

using std::vector;

void moments_memory_to_proto(vector<cleaned_moment>& internal_moments, string proto_file_path);
void moments_proto_to_memory(string proto_file_path, vector<cleaned_moment>& internal_moments);
#endif //NBA_QUERIES_CONVERT_PROTO_H
