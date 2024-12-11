#pragma once
#include "CoreTypes.h"
#include "ModuleInterface.h"

#if !WITH_EDITOR
	// If we're linking monolithically we assume all modules are linked in with the main binary.
	#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
		/** Global registrant object for this module when linked statically */ \
		static FStaticallyLinkedModuleRegistrant< ModuleImplClass > ModuleRegistrant##ModuleName( TEXT(#ModuleName) ); \
		/* Forced reference to this function is added by the linker to check that each module uses IMPLEMENT_MODULE */ \
		extern "C" void IMPLEMENT_MODULE_##ModuleName() { }
#else
	#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
	\
	/**/ \
	/* InitializeModule function, called by module manager after this module's DLL has been loaded */ \
	/**/ \
	/* @return	Returns an instance of this module */ \
	/**/ \
	extern "C" DLLEXPORT IModuleInterface* InitializeModule() \
	{ \
		return new ModuleImplClass(); \
	} \
	/* Forced reference to this function is added by the linker to check that each module uses IMPLEMENT_MODULE */ \
	extern "C" void IMPLEMENT_MODULE_##ModuleName() { }
#endif


/**
 * Function pointer type for InitializeModule().
 *
 * All modules must have an InitializeModule() function. Usually this is declared automatically using
 * the IMPLEMENT_MODULE macro below. The function must be declared using as 'extern "C"' so that the
 * name remains undecorated. The object returned will be "owned" by the caller, and will be deleted
 * by the caller before the module is unloaded.
 */
typedef IModuleInterface* (*FInitializeModuleFunctionPtr)(void);

/**
 * A default minimal implementation of a module that does nothing at startup and shutdown
 */
class FDefaultModuleImpl
	: public IModuleInterface
{ };

class FModuleManager
{
private:
	/**
	 * Information about a single module (may or may not be loaded.)
	 */
	class FModuleInfo
	{
	public:
		/** The original file name of the module, without any suffixes added */
		FString OriginalFilename;

		/** File name of this module (.dll file name) */
		FString Filename;

		/** dll 전체 경로 */
		FString FullPath;

		/** Handle to this module (DLL handle), if it's currently loaded */
		void* Handle;

		bool bDynamicLoaded = false;

		/** The module object for this module.  We actually *own* this module, so it's lifetime is controlled by the scope of this shared pointer. */
		std::unique_ptr<IModuleInterface> Module;

	};

	typedef std::shared_ptr<FModuleInfo> ModuleInfoPtr;
	/** Type definition for maps of module names to module infos. */
	typedef std::map<FName, ModuleInfoPtr> FModuleMap;

public:
	FModuleManager();
	static CORE_API FModuleManager* Get(const bool bDestroy = false);

public:
	// HBKIM: not thread safe yet. /** Thread safe module finding routine. */
	CORE_API ModuleInfoPtr FindModule(FName InModuleName);

	/**
	 * Loads the specified module.
	 *
	 * @param InModuleName The base name of the module file.  Should not include path, extension or platform/configuration info.  This is just the "module name" part of the module file name.  Names should be globally unique.
	 * @param InLoadModuleFlags Optional flags for module load operation.
	 * @return The loaded module, or nullptr if the load operation failed.
	 * @see AbandonModule, IsModuleLoaded, LoadModuleChecked, LoadModulePtr, LoadModuleWithFailureReason, UnloadModule
	 */
	CORE_API IModuleInterface* LoadModule(const FName InModuleName);

	/**
	  * Loads a module by name.
	  *
	  * @param ModuleName The module to find and load.
	  * @return The interface to the module, or nullptr if the module was not found.
	  * @see GetModulePtr, GetModuleChecked, LoadModuleChecked
	  */
	template<typename TModuleInterface>
	static TModuleInterface* LoadModulePtr(const FName InModuleName)
	{
		return static_cast<TModuleInterface*>(FModuleManager::Get()->LoadModule(InModuleName));
	}

	/**
	 * Registers an initializer for a module that is statically linked.
	 *
	 * @param InModuleName The name of this module.
	 * @param InInitializerDelegate The delegate that will be called to initialize an instance of this module.
	 */
	void RegisterStaticallyLinkedModule(const FName InModuleName, const function<IModuleInterface*(void)> InInitializerDelegate)
	{
		//PendingStaticallyLinkedModuleInitializers.Emplace(InModuleName, InInitializerDelegate);
		StaticallyLinkedModuleInitializers.insert(make_pair(InModuleName, InInitializerDelegate));
	}

	/**
	 * Adds a module to our list of modules, unless it's already known.
	 *
	 * @param InModuleName The base name of the module file.  Should not include path, extension or platform/configuration info.  This is just the "name" part of the module file name.  Names should be globally unique.
	 */
	CORE_API void AddModule(const FName InModuleName);

private:
	/** Map of module names to a delegate that can initialize each respective statically linked module */
	mutable map<FName, function<IModuleInterface*()>> StaticallyLinkedModuleInitializers;

	FModuleMap Modules;

	map<FName, FString> ModulePathsCache;
};

/**
 * Utility class for registering modules that are statically linked.
 */
template< class ModuleClass >
class FStaticallyLinkedModuleRegistrant
{
public:

	/**
	 * Explicit constructor that registers a statically linked module
	 */
	FStaticallyLinkedModuleRegistrant(FName InModuleName)
	{
		// Register this module
		FModuleManager::Get()->RegisterStaticallyLinkedModule(
			InModuleName,			// Module name
			&FStaticallyLinkedModuleRegistrant<ModuleClass>::InitializeModule);	// Initializer delegate
	}

	/**
	 * Creates and initializes this statically linked module.
	 *
	 * The module manager calls this function through the delegate that was created
	 * in the @see FStaticallyLinkedModuleRegistrant constructor.
	 *
	 * @return A pointer to a new instance of the module.
	 */
	static IModuleInterface* InitializeModule()
	{
		return new ModuleClass();
	}
};