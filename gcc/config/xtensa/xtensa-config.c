#include <config.h>
#include <system.h>
#define XTENSA_CONFIG_DEFINITION
#include "xtensa-config.h"
#include "xtensa-dynconfig.h"

static struct xtensa_config xtensa_defconfig = XTENSA_CONFIG_INITIALIZER;

void *xtensa_load_config (const char *name ATTRIBUTE_UNUSED, void *def)
{
  static int init;
#ifdef ENABLE_PLUGIN
  static void *handle;
  void *p;

  if (!init)
    {
      char *path = getenv ("XTENSA_GNU_CONFIG");

      init = 1;
      if (!path)
	return def;
      handle = dlopen (path, RTLD_LAZY);
      if (!handle)
	{
	  fprintf (stderr,
		   "XTENSA_GNU_CONFIG is defined but could not be loaded: %s\n",
		   dlerror ());
	  abort ();
	}
    }
  else if (!handle)
    {
      return def;
    }

  p = dlsym (handle, name);
  if (!p)
    {
      fprintf (stderr,
	       "XTENSA_GNU_CONFIG is loaded but symbol \"%s\" is not found: %s\n",
	       name, dlerror ());
      abort ();
    }
  return p;
#else
  if (!init)
    {
      char *path = getenv ("XTENSA_GNU_CONFIG");

      init = 1;
      if (path)
	{
	  fprintf (stderr,
		   "XTENSA_GNU_CONFIG is defined but plugin support is disabled\n");
	  abort ();
	}
    }
  return def;
#endif
}

struct xtensa_config *xtensa_get_config (void)
{
  static struct xtensa_config *config;

  if (!config)
    config = (struct xtensa_config *) xtensa_load_config ("xtensa_config",
							  &xtensa_defconfig);

  if (config->config_size < sizeof(struct xtensa_config))
    {
      fprintf (stderr,
	       "Old or incompatible configuration is loaded: config_size = %ld, expected: %ld\n",
	       config->config_size, sizeof (struct xtensa_config));
      abort ();
    }
  return config;
}
