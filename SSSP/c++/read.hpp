
#ifndef READ_HPP
#define READ_HPP

#include <vector>
#include <string>

typedef size_t node_t;
typedef std::string fact_argument;
typedef std::vector<fact_argument> fact_arguments;

typedef void (*read_handle_function)(fact_arguments);

std::vector<std::string> split(const std::string&, const std::string&);
void read_file(const std::string&);
void add_read_handler(const std::string&, read_handle_function);
node_t read_node(const fact_argument&);
int read_int(const fact_argument&);

#endif

