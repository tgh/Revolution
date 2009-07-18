/*
 * Copyright © 2009 Tyler Hayes
 * ALL RIGHTS RESERVED
 * [This program is licensed under the GPL version 3 or later.]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 *
 * This LADSPA plugin creates a fuzz distortion similar to The Beatles'
 * "Revolution".
 *
 * Thanks to Bart Massey for his direct help, Richard Furse for his examples,
 * David Benson for his tutorial, and Dave Phillips for his ladspa information.
 */

//----------------
//-- INCLUSIONS --
//----------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ladspa.h"

//-----------------------
//-- DEFINED CONSTANTS --
//-----------------------
/*
 * These are the port numbers for the plugin
 */
#define REVOLUTION_INPUT 0
#define REVOLUTION_OUTPUT 1

/*
 * Other constants
 */
#define UNIQUE_ID 3000		// the plugin's unqique ID
#define PORT_COUNT 2			// number of ports involved

//--------------------------------
//-- STRUCT FOR PORT CONNECTION --
//--------------------------------

typedef struct {
	/*
	 * This plugin does not need the sample rate from the host, nor any buffers
	 * because it is simply taking each sample and manipulating them accordingly--
	 * no time manipulation, echo, reverb, or anything that requires taking a
	 * sample and using it elsewhere in the sound.
	 */
	
	// data locations for the input & output audio ports
	LADSPA_Data * Input;
	LADSPA_Data * Output;
} Revolution;

//---------------
//-- FUNCTIONS --
//---------------

/*
 * Creates a Revolution plugin instance by allocating space for a plugin handle.
 * This function returns a LADSPA_Handle (which is a void * -- a pointer to
 * anything).
 */
LADSPA_Handle instantiate_Revolution(const LADSPA_Descriptor * descriptor, unsigned long loc_sample_rate)
{
	Revolution * revolution;	// for a Revolution struct instance
	
	// allocate space for a Revolution struct instance
	revolution = (Revolution *) malloc(sizeof(Revolution));
	
	// send the LADSPA_Handle to the host.  If malloc failed, NULL is returned.
	return revolution;
}


/*
 * Make a connection between a specified port and it's corresponding data location.
 * For example, the output port should be "connected" to the place in memory where
 * that sound data to be played is located.
 */
void connect_port_to_Revolution(LADSPA_Handle instance, unsigned long Port, LADSPA_Data * data_location)
{
	Revolution * revolution;	// for a Revolution struct instance
	
	// cast the (void *) instance to (Revolution *) and set it to local pointer
	revolution = (Revolution *) instance;
	
	// direct the appropriate data pointer to the appropriate data location
	if (Port == REVOLUTION_INPUT)
		revolution->Input = data_location;
	else if (Port == REVOLUTION_OUTPUT)
		revolution->Output = data_location;
}


/*
 * Here is where the rubber hits the road.  The actual sound manipulation
 * is done in run().  For Revolution, it merely takes each input sample, and
 * cuts it off at 0.67 (or -0.67) if it is above or below those thresholds,
 * thus "squaring off" the curved wave which produces distortion.
 */
void run_Revolution(LADSPA_Handle instance, unsigned long sample_count)
{
	LADSPA_Data * input;			// to point to the input stream
	LADSPA_Data * output;		// to point to the output stream
	Revolution * revolution;	// to set to the instance sent by the host

	revolution = (Revolution *) instance;

	// link the local pointers to their appropriate streams passed in through instance
	input = revolution->Input;
	output = revolution->Output;
	
	// for each sample, cut the value off at +/-0.67 if above or below +/-0.67.
	unsigned long i = 0;
	for (i = 0; i < sample_count; ++i)
	{
		if (*input > 0.67)
		{
			/*
			 * NOTE: (*output)++ seems more intuitive, but the compiler will bark at
			 * you for this ("lvalue required as left operand of assignment").
			 */
			*(output++) = 0.67;
			++input;
		}
		else if (*input < -0.67)
		{
			*(output++) = -0.67;
			++input;
		}
	}
}


/*
 * Frees dynamic memory associated with the Revolution instance.  The host
 * better send the right pointer in or there's gonna be a leak!
 */
void cleanup_Revolution(LADSPA_Handle instance)
{
	Revolution * revolution;	// for a Revolution struct instance
	
	// cast the (void *) instance to (Revolution *) and set it to local pointer
	revolution = (Revolution *) instance;
	
	if (revolution)
	{
		if (revolution->Input)
			free(revolution->Input);
		
		if (revolution->Output)
			free(revolution->Output);
		
		free(revolution);
	}
}


/*
 * Global LADSPA_Descriptor variable used in _init(), ladspa_descriptor(),
 * and _fini().
 */
LADSPA_Descriptor * revolution_descriptor = NULL;

/*
 * The _init() function is called whenever this plugin is first loaded
 * by the host using it.
 */
void _init()
{
	/*
	 * allocate memory for revolution_descriptor (it's just a pointer at this point).
	 * in other words create an actual LADSPA_Descriptor struct instance that
	 * revolution_descriptor will point to.
	 */
	revolution_descriptor = (LADSPA_Descriptor *) malloc(sizeof(LADSPA_Descriptor));
	
	// make sure malloc worked properly before initializing the struct fields
	if (revolution_descriptor)
	{
		// assign the unique ID of the plugin given by ?
		revolution_descriptor->UniqueID = UNIQUE_ID;	// This will be changed later when I get an offical ID <------
		
		/*
		 * assign the label of the plugin. since there are no control features for this
		 * plugin, "Revolution" is fine. (NOTE: it must not have white spaces as per ladspa.h).
		 * NOTE: in case you were wondering, strdup() from the string library makes a duplicate
		 * string of the argument and returns the duplicate's pointer (a char *).
		 */
		revolution_descriptor->Label = strdup("Revolution_Distortion");
		
		/*
		 * assign the special property of the plugin, which is any of the three
		 * defined in ladspa.h: LADSPA_PROPERTY_REALTIME, LADSPA_PROPERTY_INPLACE_BROKEN,
		 * and LADSPA_PROPERTY_HARD_RT_CAPABLE.  They are just ints (1, 2, and 4,
		 * respectively).  See ladspa.h for what they actually mean.
		 */
		revolution_descriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		
		// assign the plugin name
		revolution_descriptor->Name = strdup("Revolution");
		
		// assign the author of the plugin
		revolution_descriptor->Maker = strdup("Tyler Hayes");
		
		/*
		 * assign the copyright info of the plugin (NOTE: use "None" for no copyright
		 * as per ladspa.h)
		 */
		revolution_descriptor->Copyright = strdup("GPL");
		
		/*
		 * assign the number of ports for the plugin.  since there are no control
		 * features for Revolution, there are only 2 ports: audio input and output.
		 */
		revolution_descriptor->PortCount = PORT_COUNT;
		
		LADSPA_PortDescriptor * temp_descriptor_array;	// used for allocating and initailizing a
																		// LADSPA_PortDescriptor array (which is
																		// an array of ints) since revolution_descriptor->
																		// PortDescriptors is a const *.
		
		// allocate space for the temporary array with a length of the number of ports (PortCount)
		temp_descriptor_array = (LADSPA_PortDescriptor *) calloc(PORT_COUNT, sizeof(LADSPA_PortDescriptor));
		
		/*
		* set the instance LADSPA_PortDescriptor array (PortDescriptors) pointer to
		* the location temp_descriptor_array is pointing at.
		*/
		revolution_descriptor->PortDescriptors = (const LADSPA_PortDescriptor *) temp_descriptor_array;
		
		/*
		 * set the port properties by ORing specific bit masks defined in ladspa.h.
		 *
		 * this first one gives the first port the properties that tell the host that
		 * this port takes input and is an audio port (not a control port).
		 */
		temp_descriptor_array[REVOLUTION_INPUT] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		
		/*
		 * this gives the second port the properties that tell the host that this port is
		 * an output port and that it is an audio port (I don't see any situation where
		 * one might be an output port, but not an audio port...).
		 */
		temp_descriptor_array[REVOLUTION_OUTPUT] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		
		/*
		 * set temp_descriptor_array to NULL for housekeeping--we don't need that local
		 * variable anymore.
		 */
		temp_descriptor_array = NULL;
		
		char ** temp_port_names;	// temporary local variable (which is a pointer to an array
											// of arrays of characters) for the names of the ports since
											// revolution_descriptor->PortNames is a const char * const *.

		// allocate the space for two port names
		temp_port_names = (char **) calloc(PORT_COUNT, sizeof(char *));
		
		/*
		* set the instance PortNames array pointer to the location temp_port_names
		* is pointing at.
		*/
		revolution_descriptor->PortNames = (const char **) temp_port_names;
		
		// set the name of the input port
		temp_port_names[REVOLUTION_INPUT] = strdup("Input");
		
		// set the name of the ouput port
		temp_port_names[REVOLUTION_OUTPUT] = strdup("Output");
		
		// reset temp variable to NULL for housekeeping
		temp_port_names = NULL;
		
		LADSPA_PortRangeHint * temp_hints;	// temporary local variable (pointer to a
														// PortRangeHint struct) since revolution_descriptor->
														// PortRangeHints is a const *.
		
		// allocate space for two port hints (see ladspa.h for info on 'hints')									
		temp_hints = (LADSPA_PortRangeHint *) calloc(PORT_COUNT, sizeof(LADSPA_PortRangeHint));
		
		/*
		* set the instance PortRangeHints pointer to the location temp_hints
		* is pointed at.
		*/
		revolution_descriptor->PortRangeHints = (const LADSPA_PortRangeHint *) temp_hints;
		
		/*
		 * set the port hint descriptors (which are ints). Since this is a simple
		 * distortion effect without control, input and ouput don't need any range hints.
		 */
		temp_hints[REVOLUTION_INPUT].HintDescriptor = 0;
		temp_hints[REVOLUTION_OUTPUT].HintDescriptor = 0;
		
		// reset temp variable to NULL for housekeeping
		temp_hints = NULL;
		
		// set the instance's function pointers to appropriate functions
		revolution_descriptor->instantiate = instantiate_Revolution;
		revolution_descriptor->connect_port = connect_port_to_Revolution;
		
		/*
		 * activate is not needed because buffers are not used and thus don't
		 * need to be cleared, which is what activate essentially does (I think?).
		 */
		revolution_descriptor->activate = NULL;
		
		revolution_descriptor->run = run_Revolution;
		revolution_descriptor->run_adding = NULL;
		revolution_descriptor->set_run_adding_gain = NULL;
		revolution_descriptor->deactivate = NULL;
		revolution_descriptor->cleanup = cleanup_Revolution;
	}
	
}


/*
 * Returns a descriptor of the requested plugin type (there is only one plugin
 * type in this library).
 *
 * NOTE: this function MUST be called 'ladspa_descriptor' or else the plugin
 * will not be recognized.
 */
const LADSPA_Descriptor * ladspa_descriptor(unsigned long index)
{
	if (index == 0)
		return revolution_descriptor;
	else
		return NULL;
}


/*
 * This is called automatically by the host when it is done with this plugin.
 * (When this dynamic library is unloaded).  It frees all dynamically
 * allocated memory associated with the descriptor.
 */
void _fini()
{
	if (revolution_descriptor)
	{
		free((char *) revolution_descriptor->Label);
		free((char *) revolution_descriptor->Name);
		free((char *) revolution_descriptor->Maker);
		free((char *) revolution_descriptor->Copyright);
		free((LADSPA_PortDescriptor *) revolution_descriptor->PortDescriptors);
		
		/*
		 * the for loop here is kind of unnecessary since the number of ports
		 * was hard coded for this plugin as 2, but whatever.
		 */
		int i = 0;
		for(i = 0; i < revolution_descriptor->PortCount; ++i)
			free((char *)(revolution_descriptor->PortNames[i]));
		
		free((char **) revolution_descriptor->PortNames);
		free((LADSPA_PortRangeHint *) revolution_descriptor->PortRangeHints);
		
		free(revolution_descriptor);
	}
}
