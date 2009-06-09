//-----------------------------------------------------------------------------
// Product:     OpenCTM
// File:        openctm.h
// Description: OpenCTM API definition.
//-----------------------------------------------------------------------------
// Copyright (c) 2009 Marcus Geelnard
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source
//     distribution.
//-----------------------------------------------------------------------------

#ifndef __OPENCTM_H_
#define __OPENCTM_H_

/*! @mainpage OpenCTM Documentation
 *
 * @section intro_sec Introduction
 *
 * OpenCTM is an open file format for storing compressed triangle meshes.
 * In order to easily read and write OpenCTM files (usually suffixed .ctm) an
 * API (Application Program Interface) is provided that can easily be used from
 * most modern programming languages.
 *
 * The OpenCTM functionality itself is written in highly portable standard C
 * (C99).
 *
 * @section usage_sec Usage
 *
 * For information about how to use the OpenCTM API, see openctm.h.
 *
 * For information about the C++ wrapper classes, see CTMimporter and
 * CTMexporter.
 *
 * @section example_sec Example usage
 *
 * @subsection example_load_sec Loading a CTM file
 *
 * Here is a simple example of loading a CTM file:
 *
 * @code
 *   CTMcontext context;
 *   CTMuint    vertCount, triCount, * indices;
 *   CTMfloat   * vertices;
 *
 *   // Create a new context
 *   context = ctmNewContext(CTM_IMPORT);
 *
 *   // Load the OpenCTM file
 *   ctmLoad(context, "mymesh.ctm");
 *   if(ctmGetError(context) == CTM_NONE)
 *   {
 *     // Access the mesh data
 *     vertCount = ctmGetInteger(context, CTM_VERTEX_COUNT);
 *     vertices = ctmGetFloatArray(context, CTM_VERTICES);
 *     triCount = ctmGetInteger(context, CTM_TRIANGLE_COUNT);
 *     indices = ctmGetIntegerArray(context, CTM_INDICES);
 *
 *     // Deal with the mesh (e.g. transcode it to our internal representation)
 *     // ...
 *   }
 *
 *   // Free the context
 *   ctmFreeContext(context);
 * @endcode
 *
 * @subsection example_create_sec Creating a CTM file
 *
 * Here is a simple example of creating a CTM file:
 *
 * @code
 *   CTMcontext context;
 *   CTMuint    vertCount, triCount, * indices;
 *   CTMfloat   * vertices;
 *
 *   // Create our mesh in memory
 *   vertCount = 100;
 *   triCount = 120;
 *   vertices = (CTMfloat *) malloc(3 * sizeof(CTMfloat) * vertCount);
 *   indices = (CTMuint *) malloc(3 * sizeof(CTMuint) * triCount);
 *   // ...
 *
 *   // Create a new context
 *   context = ctmNewContext(CTM_EXPORT);
 *
 *   // Define our mesh representation to OpenCTM (store references to it in
 *   // the context)
 *   ctmDefineMesh(context, vertices, vertCount, indices, triCount, NULL);
 *
 *   // Save the OpenCTM file
 *   ctmSave(context, "mymesh.ctm");
 *
 *   // Free the context
 *   ctmFreeContext(context);
 *
 *   // Free our mesh
 *   free(indices);
 *   free(vertices);
 * @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif


// Declare calling conventions etc.
#if defined(WIN32) || defined(_WIN32)
  // Windows
  #if defined(OPENCTM_STATIC)
    #define CTMEXPORT
    #define CTMCALL
  #else
    #if defined(OPENCTM_BUILD)
      #define CTMEXPORT __declspec(dllexport)
    #else
      #define CTMEXPORT __declspec(dllimport)
    #endif
    #if defined(__MINGW32__)
      #define CTMCALL __attribute__ ((__stdcall__))
    #elif (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
      #define CTMCALL __stdcall
    #else
      #define CTMCALL
    #endif
  #endif
#else
  // Unix
  #if !defined(OPENCTM_STATIC) && !defined(OPENCTM_BUILD)
    #define CTMEXPORT extern
  #else
    #if defined(OPENCTM_BUILD) && defined(__GNUC__) && (__GNUC__ >= 4)
      #define CTMEXPORT __attribute__ ((visibility("default")))
    #else
      #define CTMEXPORT
    #endif
  #endif
  #define CTMCALL
#endif


// Get system specific type definitions for sized integers. We use the C99
// standard stdint.h for this.
#ifdef _MSC_VER
// MS Visual Studio does not support C99
typedef int int32_t;
typedef unsigned int uint32_t;
#else
#include <stdint.h>
#endif


/// OpenCTM API version (0.4).
#define CTM_API_VERSION 0x00000004

/// Boolean TRUE.
#define CTM_TRUE 1

/// Boolean FALSE.
#define CTM_FALSE 0

/// Single precision floating point type (IEEE 754 32 bits wide).
typedef float CTMfloat;

/// Signed integer (32 bits wide).
typedef int32_t CTMint;

/// Unsigned integer (32 bits wide).
typedef uint32_t CTMuint;

/// OpenCTM context handle.
typedef void * CTMcontext;

/// OpenCTM specific enumerators.
/// @note For the information query functions, it is an error to query a value
///       of the wrong type (e.g. to query a string value with the
///       ctmGetInteger() function).
typedef enum {
  // Error codes (see ctmGetError())
  CTM_NONE              = 0x0000, ///< No error has occured (everything is OK).
                                  ///  Also used as an error return value for
                                  ///  functions that should return a CTMenum
                                  ///  value.
  CTM_INVALID_CONTEXT   = 0x0001, ///< The OpenCTM context was invalid (e.g. NULL).
  CTM_INVALID_ARGUMENT  = 0x0002, ///< A function argument was invalid.
  CTM_INVALID_OPERATION = 0x0003, ///< The operation is not allowed.
  CTM_INVALID_MESH      = 0x0004, ///< The mesh was invalid (e.g. no vertices).
  CTM_OUT_OF_MEMORY     = 0x0005, ///< Not enough memory to proceed.
  CTM_FILE_ERROR        = 0x0006, ///< File I/O error.
  CTM_FORMAT_ERROR      = 0x0007, ///< File format error (e.g. unrecognized format).
  CTM_LZMA_ERROR        = 0x0008, ///< An error occured within the LZMA library.
  CTM_INTERNAL_ERROR    = 0x0009, ///< An internal error occured (indicates a bug).

  // OpenCTM context modes
  CTM_IMPORT            = 0x0101, ///< The OpenCTM context will be used for importing data.
  CTM_EXPORT            = 0x0102, ///< The OpenCTM context will be used for exporting data.

  // Compression methods
  CTM_METHOD_RAW        = 0x0201, ///< Just store the raw data.
  CTM_METHOD_MG1        = 0x0202, ///< Lossless compression (floating point).
  CTM_METHOD_MG2        = 0x0203, ///< Lossless compression (fixed point).

  // Integer queries
  CTM_VERTEX_COUNT      = 0x0301, ///< Number of vertices in the mesh (integer).
  CTM_TRIANGLE_COUNT    = 0x0302, ///< Number of triangles in the mesh (integer).
  CTM_HAS_NORMALS       = 0x0303, ///< CTM_TRUE if the mesh has normals (integer).
  CTM_TEX_MAP_COUNT     = 0x0304, ///< Number of texture coordinate sets (integer).
  CTM_ATTRIB_MAP_COUNT  = 0x0305, ///< Number of custom attribute sets (integer).

  // String queries
  CTM_FILE_COMMENT      = 0x0401, ///< File comment (string).

  // Texture map queries
  CTM_FILE_NAME         = 0x0501, ///< File name reference (texture map string).

  // Array queries
  CTM_INDICES           = 0x0601, ///< Triangle indices (integer array).
  CTM_VERTICES          = 0x0602, ///< Vertex point coordinates (float array).
  CTM_NORMALS           = 0x0603, ///< Per vertex normals (float array).
  CTM_TEX_MAP_1         = 0x0700, ///< Per vertex texture map 1 (float array).
  CTM_TEX_MAP_2         = 0x0701, ///< Per vertex texture map 2 (float array).
  CTM_TEX_MAP_3         = 0x0702, ///< Per vertex texture map 3 (float array).
  CTM_TEX_MAP_4         = 0x0703, ///< Per vertex texture map 4 (float array).
  CTM_TEX_MAP_5         = 0x0704, ///< Per vertex texture map 5 (float array).
  CTM_TEX_MAP_6         = 0x0705, ///< Per vertex texture map 6 (float array).
  CTM_TEX_MAP_7         = 0x0706, ///< Per vertex texture map 7 (float array).
  CTM_TEX_MAP_8         = 0x0707, ///< Per vertex texture map 8 (float array).
  CTM_ATTRIB_MAP_1      = 0x0800, ///< Per vertex attribute map 1 (float array).
  CTM_ATTRIB_MAP_2      = 0x0801, ///< Per vertex attribute map 2 (float array).
  CTM_ATTRIB_MAP_3      = 0x0802, ///< Per vertex attribute map 3 (float array).
  CTM_ATTRIB_MAP_4      = 0x0803, ///< Per vertex attribute map 4 (float array).
  CTM_ATTRIB_MAP_5      = 0x0804, ///< Per vertex attribute map 5 (float array).
  CTM_ATTRIB_MAP_6      = 0x0805, ///< Per vertex attribute map 6 (float array).
  CTM_ATTRIB_MAP_7      = 0x0806, ///< Per vertex attribute map 7 (float array).
  CTM_ATTRIB_MAP_8      = 0x0807  ///< Per vertex attribute map 8 (float array).
} CTMenum;

/// Stream read() function pointer.
/// @param[in] aBuf Pointer to the memory buffer to which data should be read.
/// @param[in] aCount The number of bytes to read.
/// @param[in] aUserData The custom user data that was passed to the
///            ctmLoadCustom() function.
/// @return The number of bytes actually read (if this is less than aCount, it
///         indicates that an error occured or the end of file was reached).
typedef CTMuint (* CTMreadfn)(void * aBuf, CTMuint aCount, void * aUserData);

/// Stream write() function pointer.
/// @param[in] aBuf Pointer to the memory buffer from which data should be written.
/// @param[in] aCount The number of bytes to write.
/// @param[in] aUserData The custom user data that was passed to the
///            ctmSaveCustom() function.
/// @return The number of bytes actually written (if this is less than aCount, it
///         indicates that an error occured).
typedef CTMuint (* CTMwritefn)(const void * aBuf, CTMuint aCount, void * aUserData);

/// Create a new OpenCTM context. The context is used for all subsequent
/// OpenCTM function calls. Several contexts can coexist at the same time.
/// @param[in] aMode An OpenCTM context mode. Set this to CTM_IMPORT if the
///            context will be used for importing data, or set it to CTM_EXPORT
///            if it will be used for exporting data.
/// @return An OpenCTM context handle (or NULL if no context could be created).
CTMEXPORT CTMcontext CTMCALL ctmNewContext(CTMenum aMode);

/// Free an OpenCTM context.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @see ctmNewContext()
CTMEXPORT void CTMCALL ctmFreeContext(CTMcontext aContext);

/// Returns the latest error. Calling this function will return the last
/// produced error code, or CTM_NO_ERROR (zero) if no error has occured since
/// the last call to ctmGetError(). When this function is called, the internal
/// error varibale will be reset to CTM_NO_ERROR.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @return An OpenCTM error code.
/// @see CTMenum
CTMEXPORT CTMenum CTMCALL ctmGetError(CTMcontext aContext);

/// Get information about an OpenCTM context.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aProperty Which property to return.
/// @return An integer value, representing the OpenCTM context property given
///         by \c aProperty.
/// @see CTMenum
CTMEXPORT CTMuint CTMCALL ctmGetInteger(CTMcontext aContext, CTMenum aProperty);

/// Get an integer array from an OpenCTM context.
/// @param[in] aContext An OpenCTM context that has been created by
///             ctmNewContext().
/// @param[in] aProperty Which array to return.
/// @return An integer array. If the requested array does not exist, or
///         if \c aProperty does not indicate an integer array, the function
///         returns NULL.
/// @note The array is only valid as long as the OpenCTM context is valid, or
///       until the corresponding array changes within the OpenCTM contxt
///       Trying to access an invalid array will result in undefined
///       behaviour. Therefor it is recommended that the array is copied to
///       a new variable if it is to be used other than directly after the call
///       to ctmGetIntegerArray().
/// @see CTMenum
CTMEXPORT const CTMuint * CTMCALL ctmGetIntegerArray(CTMcontext aContext,
  CTMenum aProperty);

/// Get a floating point array from an OpenCTM context.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aProperty Which array to return.
/// @return A floating point array. If the requested array does not exist, or
///         if \c aProperty does not indicate a float array, the function
///         returns NULL.
/// @note The array is only valid as long as the OpenCTM context is valid, or
///       until the corresponding array changes within the OpenCTM contxt
///       Trying to access an invalid array will result in undefined
///       behaviour. Therefor it is recommended that the array is copied to
///       a new variable if it is to be used other than directly after the call
///       to ctmGetFloatArray().
/// @see CTMenum
CTMEXPORT const CTMfloat * CTMCALL ctmGetFloatArray(CTMcontext aContext,
  CTMenum aProperty);

/// Get a reference to the named texture map.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aName The name of the texture map that should be returned.
/// @return A reference to a texture map. If the texture map was found,
///          a value of CTM_TEX_MAP_1 or higher is returned, otherwise CTM_NONE
///          is returned.
CTMEXPORT CTMenum CTMCALL ctmGetNamedTexMap(CTMcontext aContext,
  const char * aName);

/// Get information about a texture map.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aTexMap Which texture map to query (CTM_TEX_MAP_1 or higher).
/// @param[in] aProperty Which texture map property to return.
/// @return A string value, representing the texture map property given
///         by \c aProperty.
/// @note The string is only valid as long as the texture map within the OpenCTM
///       context is valid. Trying to access an invalid string will result in
///       undefined behaviour. Therefor it is recommended that the string is
///       copied to a new variable if it is to be used other than directly after
///       the call to ctmGetTexMapString().
/// @see CTMenum
CTMEXPORT const char * CTMCALL ctmGetTexMapString(CTMcontext aContext,
  CTMenum aTexMap, CTMenum aProperty);

/// Get a reference to the named vertex attribute map.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aName The name of the attribute map that should be returned.
/// @return A reference to an attribute map. If the attribute map was found,
///         a value of CTM_ATTRIB_MAP_1 or higher is returned, otherwise
///         CTM_NONE is returned.
CTMEXPORT CTMenum CTMCALL ctmGetNamedAttribMap(CTMcontext aContext,
  const char * aName);

/// Get information about an OpenCTM context.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aProperty Which property to return.
/// @return A string value, representing the OpenCTM context property given
///         by \c aProperty.
/// @note The string is only valid as long as the OpenCTM context is valid, or
///       until the corresponding string changes within the OpenCTM contxt
///       (e.g. calling ctmFileComment() invalidates the CTM_FILE_COMMENT
///       string). Trying to access an invalid string will result in undefined
///       behaviour. Therefor it is recommended that the string is copied to
///       a new variable if it is to be used other than directly after the call
///       to ctmGetString().
/// @see CTMenum
CTMEXPORT const char * CTMCALL ctmGetString(CTMcontext aContext,
  CTMenum aProperty);

/// Set which compression method to use for the given OpenCTM context.
/// The selected compression method will be used when calling the ctmSave()
/// function.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aMethod Which compression method to use: CTM_METHOD_RAW,
///            CTM_METHOD_MG1 or CTM_METHOD_MG2 (the default method is
///            CTM_METHOD_MG1).
/// @see CTM_METHOD_RAW, CTM_METHOD_MG1, CTM_METHOD_MG2
CTMEXPORT void CTMCALL ctmCompressionMethod(CTMcontext aContext,
  CTMenum aMethod);

/// Set the vertex coordinate precision (only used by the MG2 compression
/// method).
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aPrecision Fixed point precision. For instance, if this value is
///            0.001, all vertex coordinates will be rounded to three decimals.
///            The default vertex coordinate precision is 2^-10 ~= 0.00098.
CTMEXPORT void CTMCALL ctmVertexPrecision(CTMcontext aContext,
  CTMfloat aPrecision);

/// Set the vertex coordinate precision, relative to the mesh dimensions (only
/// used by the MG2 compression method).
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aRelPrecision Relative precision. This factor is multiplied by the
///            average triangle edge length in the mesh in order to obtain the
///            final, fixed point precision. For instance, if aRelPrecision is 
///            0.01, and the average edge length is 3.7, then the fixed point
///            precision is set to 0.037.
/// @note The mesh must have been defined using the ctmDefineMesh() function
///       before calling this function.
/// @see ctmVertexPrecision().
CTMEXPORT void CTMCALL ctmVertexPrecisionRel(CTMcontext aContext,
  CTMfloat aRelPrecision);

/// Set the normal precision (only used by the MG2 compression method). The
/// normal is represented in spherical coordinates in the MG2 compression
/// method, and the normal precision controls the angular and radial resolution.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aPrecision Fixed point precision. For the angular information,
///            this value represents the angular precision. For the radial
///            information, this value is the linear resolution. For instance,
///            0.01 means that the circle is divided into 100 steps, and the
///            normal magnitude is rounded to 2 decimals. The default normal
///            precision is 2^-8 ~= 0.0039.
CTMEXPORT void CTMCALL ctmNormalPrecision(CTMcontext aContext,
  CTMfloat aPrecision);

/// Set the texture coordinate precision for the specified texture map (only
/// used by the MG2 compression method).
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aTexMap A texture map specifier for a defined texture map
///            (CTM_TEX_MAP_1, ...).
/// @param[in] aPrecision Fixed point precision. For instance, if this value is
///            0.001, all texture coordinates will be rounded to three decimals.
///            The default texture coordinate precision is 2^-12 ~= 0.00024.
/// @see ctmAddTexMap().
CTMEXPORT void CTMCALL ctmTexCoordPrecision(CTMcontext aContext,
  CTMenum aTexMap, CTMfloat aPrecision);

/// Set the attribute value precision for the specified attribute map (only
/// used by the MG2 compression method).
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aAttribMap An attribute map specifier for a defined attribute map
///            (CTM_ATTRIB_MAP_1, ...).
/// @param[in] aPrecision Fixed point precision. For instance, if this value is
///            0.001, all attribute values will be rounded to three decimals.
///            If the attributes represent integer values, set the precision
///            to 1.0. The default attribute precision is 2^-8 ~= 0.0039.
/// @see ctmAddAttribMap().
CTMEXPORT void CTMCALL ctmAttribPrecision(CTMcontext aContext,
  CTMenum aAttribMap, CTMfloat aPrecision);

/// Set the file comment for the given OpenCTM context.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aFileComment The file comment (zero terminated UTF-8 string).
CTMEXPORT void CTMCALL ctmFileComment(CTMcontext aContext,
  const char * aFileComment);

/// Define a triangle mesh.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aVertices An array of vertices (three consecutive floats make
///            one vertex).
/// @param[in] aVertexCount The number of vertices in \c aVertices (and
///            optionally \c aTexCoords).
/// @param[in] aIndices An array of vertex indices (three consecutive integers
///            make one triangle).
/// @param[in] aTriangleCount The number of triangles in \c aIndices (there
///            must be exactly 3 x \c aTriangleCount indices in \c aIndices).
/// @param[in] aNormals An array of per-vertex normals (or NULL if there are
///            no normals). Each nromal is made up by three consecutive floats,
///            and there must be \c aVertexCount normals.
/// @see ctmAddTexMap(), ctmAddAttribMap(), ctmSave(), ctmSaveCustom().
CTMEXPORT void CTMCALL ctmDefineMesh(CTMcontext aContext,
  const CTMfloat * aVertices, CTMuint aVertexCount, const CTMuint * aIndices,
  CTMuint aTriangleCount, const CTMfloat * aNormals);

/// Define a texture map. There can be several texture maps in a mesh. The first
/// defined texture map is considered the "primary" texture map. Many
/// applications only support a single texture map, and the recommended
/// behaviour is for them to extrect the primary texture map from an OpenCTM
/// file.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aTexCoords An array of texture coordinates. Each texture
///            coordinate is made up by two consecutive floats, and there must
///            be as many coordinates as there are vertices in the mesh.
/// @param[in] aName A unique name for this texture map (zero terminated UTF-8
///            string). It is recommended that this name is human readable
///            (e.g. "Pigment" or "Normal map").
/// @param[in] aFileName A reference to a texture image file (zero terminated
///            UTF-8 string). If no file name reference exists, pass NULL.
/// @return A texture map index (CTM_TEX_MAP_1 and higher). If the function
///         failed, it will return the zero valued CTM_NONE (use ctmGetError()
///         to determine the cause of the error).
/// @note A triangle mesh must have been defined before calling this function,
///       since the number of vertices is defined by the triangle mesh.
/// @see ctmDefineMesh().
CTMEXPORT CTMenum CTMCALL ctmAddTexMap(CTMcontext aContext,
  const CTMfloat * aTexCoords, const char * aName, const char * aFileName);

/// Define a custom vertex attribute map. Custom vertex attributes can be used
/// for defining special per-vertex attributes, such as color, weight, ambient
/// occlusion factor, etc.
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aAttribValues An array of attribute values. Each attribute value
///            is made up by four consecutive floats, and there must be as many
///            values as there are vertices in the mesh.
/// @param[in] aName A unique name for this attribute map (zero terminated UTF-8
///            string). It is recommended that this name is human readable
///            (e.g. "Color" or "Ambient occlusion").
/// @return A attribute map index (CTM_ATTRIB_MAP_1 and higher). If the function
///         failed, it will return the zero valued CTM_NONE (use ctmGetError()
///         to determine the cause of the error).
/// @note A triangle mesh must have been defined before calling this function,
///       since the number of vertices is defined by the triangle mesh.
/// @see ctmDefineMesh().
CTMEXPORT CTMenum CTMCALL ctmAddAttribMap(CTMcontext aContext,
  const CTMfloat * aAttribValues, const char * aName);

/// Load an OpenCTM format file. The mesh can be retrieved using ctmGetMesh().
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aFileName The name of the file to be loaded.
CTMEXPORT void CTMCALL ctmLoad(CTMcontext aContext, const char * aFileName);

/// Load an OpenCTM format file using a custom stream read function. The mesh
/// can be retrieved using ctmGetMesh().
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aReadFn Pointer to a custom stream read function.
/// @param[in] aUserData Custom user data, which can be a C language FILE
///            handle, C++ istream object, or a custom object pointer
///            of any type. The user data pointer will be passed to the
///            custom stream read function.
/// @see CTMreadfn.
CTMEXPORT void CTMCALL ctmLoadCustom(CTMcontext aContext, CTMreadfn aReadFn,
  void * aUserData);

/// Save an OpenCTM format file. The mesh must have been defined by
/// ctmDefineMesh().
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aFileName The name of the file to be saved.
CTMEXPORT void CTMCALL ctmSave(CTMcontext aContext, const char * aFileName);

/// Save an OpenCTM format file using a custom stream write function. The mesh
/// must have been defined by ctmDefineMesh().
/// @param[in] aContext An OpenCTM context that has been created by
///            ctmNewContext().
/// @param[in] aWriteFn Pointer to a custom stream write function.
/// @param[in] aUserData Custom user data, which can be a C language FILE
///            handle, C++ ostream object, or a custom object pointer
///            of any type. The user data pointer will be passed to the
///            custom stream write function.
/// @see CTMwritefn.
CTMEXPORT void CTMCALL ctmSaveCustom(CTMcontext aContext, CTMwritefn aWriteFn,
  void * aUserData);

#ifdef __cplusplus
}
#endif


// C++ extensions to the API
#ifdef __cplusplus
#include "openctmpp.h"
#endif

#endif // __OPENCTM_H_
