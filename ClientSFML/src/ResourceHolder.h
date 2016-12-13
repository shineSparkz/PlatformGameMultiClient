#ifndef __RESOURCE_HOLDER_H__
#define __RESOURCE_HOLDER_H__

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

#include "LogFile.h"
#include "types.h"

template <typename Resource, typename Identifier>
class ResourceHolder
{
public:
	~ResourceHolder()
	{
		m_resource_map.clear();
	}

	int Load(Identifier id, const std::string& filepath );

	template <typename Parameter>
	int Load(Identifier id, const std::string& filename, const Parameter& secondParam);

	Resource& Get(Identifier id);
	const Resource& Get(Identifier id) const;

private:
	int InsertResource(Identifier id, std::unique_ptr<Resource> resource, const std::string& name);

private:
	std::map<Identifier, std::unique_ptr<Resource>> m_resource_map;
};


template <typename Resource, typename Identifier >
int ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& file)
{
	//Create and load the reource
	std::unique_ptr<Resource> res(new Resource() );
	if ( !res->loadFromFile(file) )
	{
		WRITE_LOG("Resource Holder Load failed: failed to load: " + file, "error");
		return GE_FATAL_ERROR;
	}

	return InsertResource( id, std::move( res ), file );
}

template <typename Resource, typename Identifier>
template <typename Parameter>
int ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename, const Parameter& secondParam)
{
	// Create and load resource
	std::unique_ptr<Resource> resource(new Resource());
	if ( !resource->loadFromFile(filename, secondParam) )
	{
		WRITE_LOG("ResourceHolder::load - Failed to load " + filename, "error");
		return GE_FATAL_ERROR;
	}

	// If loading successful, insert resource to map
	return InsertResource(id, std::move(resource), filename);
}

template <typename Resource, typename Identifier>
Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id)
{
	auto found = m_resource_map.find(id);
	assert(found != m_resource_map.end());
	return *found->second;
}

template <typename Resource, typename Identifier>
const Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id) const
{
	auto found = m_resource_map.find(id);
	assert(found != m_resource_map.end());
	return *found->second;
}

template <typename Resource, typename Identifier>
int ResourceHolder<Resource, Identifier>::InsertResource(Identifier id, std::unique_ptr<Resource> resource, const std::string& name)
{
	// Insert and check success
	auto inserted = m_resource_map.insert(std::make_pair(id, std::move(resource)));

	if (!inserted.second)
	{
		WRITE_LOG("Error: The resource: " + name + " could not be added to resource map", "error");
		return GE_FATAL_ERROR;
	}

	WRITE_LOG("Info: The resource: " + name + " added to resource map...", "good");
	return GE_OK;
}

#endif