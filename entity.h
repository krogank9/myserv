#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "arg_stream.h"

#define SUBCLASS_ENTITY(child_class)\
	static entity* recreate_from_stream(arg_stream* stream) { return set_props_from_stream(new child_class()); }\
	static std::string get_class_name() { return "child_class"; }

class entity
{
public:
	entity();
	static entity* set_props_from_stream(entity* ent, arg_stream* stream) { return ent; }
	static entity* recreate_from_stream(arg_stream* stream) { return set_props_from_stream(new entity(), NULL); }
	static std::string get_class_name() { return "entity"; }
};

#endif // ENTITY_H
