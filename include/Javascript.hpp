#ifndef OSMIUM_JAVASCRIPT_HPP
#define OSMIUM_JAVASCRIPT_HPP

#include "v8.h"
#include <unicode/ustring.h>

#include "JavascriptTemplate.hpp"

/* These unicode conversion functions are used to convert UTF-8 to UTF-16 and then into
   a v8::String and back, because the functions that v8 has for this do not handle
   characters outside the Basic Multilingual Plane (>16bit) properly. */

/// Parent exception class for Unicode conversion errors.
class Unicode_Conversion_Error {

  public:
        
    UErrorCode error_code;
    Unicode_Conversion_Error(UErrorCode ec) : error_code(ec) { }

    /// Is this a buffer overflow?
    bool buffer_overflow() {
        return error_code == U_BUFFER_OVERFLOW_ERROR;
    }

};

/// Exception thrown when a UTF-8 to UTF-16 conversion failed.
class UTF8_to_UTF16_Conversion_Error : public Unicode_Conversion_Error {

  public:

    UTF8_to_UTF16_Conversion_Error(UErrorCode ec) : Unicode_Conversion_Error(ec) { }

};

/// Exception thrown when a UTF-16 to UTF-8 conversion failed.
class UTF16_to_UTF8_Conversion_Error : public Unicode_Conversion_Error {

  public:

    UTF16_to_UTF8_Conversion_Error(UErrorCode ec) : Unicode_Conversion_Error(ec) { }

};

/**
 * Convert C string with UTF-8 codes into v8::String.
 *
 * @exception UTF8_to_UTF16_Conversion_Error Thrown if the conversion failed.
 * @param characters Maximum number of Unicode characters.
 * @param cstring A NULL terminated C string.
 * @return A local handle to a v8 String.
 */
template<int characters> v8::Local<v8::String> utf8_to_v8_String(const char *cstring) {
    UErrorCode error_code = U_ZERO_ERROR;
    UChar dest[characters*2];
    int32_t dest_length;
    u_strFromUTF8(dest, characters*2, &dest_length, cstring, -1, &error_code);
    if (error_code != U_ZERO_ERROR) {
        throw UTF8_to_UTF16_Conversion_Error(error_code);
    }
    return v8::String::New(dest, dest_length);
}

/**
 * Convert v8::String into C string with UTF-8 codes.
 *
 * @exception UTF16_to_UTF8_Conversion_Error Thrown if the conversion failed.
 * @param characters Maximum number of Unicode characters.
 * @param string A v8::String.
 * @return Returns a pointer to a static buffer with a NULL terminated C string.
 */
template<int characters> const char *v8_String_to_utf8(v8::Local<v8::String> string) {
    UErrorCode error_code = U_ZERO_ERROR;
    uint16_t src[characters*2];
    static char buffer[characters*4];
    int32_t buffer_length;
    string->Write(src, 0, characters*2);
    u_strToUTF8(buffer, characters*4, &buffer_length, src, std::min(characters*2, string->Length()), &error_code);
    if (error_code != U_ZERO_ERROR) {
        throw UTF16_to_UTF8_Conversion_Error(error_code);
    }
    return buffer;
}

// this function does not work without the inline. strange.
/**
 * Sends v8::String to output stream. This will first convert it to a UTF-8 string.
 *
 * @exception UTF16_to_UTF8_Conversion_Error Thrown if the conversion failed.
 * @param string A v8::String.
 * @param os A reference to an output stream.
 */
inline void v8_String_to_ostream(v8::Local<v8::String> string, std::ostream &os) {
    UErrorCode error_code = U_ZERO_ERROR;
    int length = 4 * (string->Length() + 1);
    uint16_t *src = (uint16_t *) malloc(length);
    if (!src) {
        throw std::bad_alloc();
    }
    char *buffer = (char *) malloc(length);
    if (!buffer) {
        throw std::bad_alloc();
    }
    int32_t buffer_length;
    string->Write(src);
    u_strToUTF8(buffer, length, &buffer_length, src, string->Length(), &error_code);
    if (error_code != U_ZERO_ERROR) {
        throw UTF16_to_UTF8_Conversion_Error(error_code);
    }
    os << buffer;
    free(buffer);
    free(src);
}

namespace Osmium {

    namespace Javascript {

        namespace Object {

            class Wrapper {

                Osmium::OSM::Object *object;

                public:

                v8::Local<v8::Object> js_object_instance;
                v8::Local<v8::Object> js_tags_instance;

                virtual Osmium::OSM::Object *get_object() {
                    return object;
                }

                protected:

                Wrapper() {
                }

                public:

                v8::Local<v8::Object> get_instance() {
                    return js_object_instance;
                }

            }; // class Wrapper

        } // namespace Object

        namespace Node {

            class Wrapper : public Osmium::Javascript::Object::Wrapper {

                public:

                Osmium::OSM::Node *object;

                v8::Local<v8::Object> js_geom_instance;

                Wrapper() : Osmium::Javascript::Object::Wrapper() {
                    object = new Osmium::OSM::Node;
                    object->wrapper = this;
                    js_tags_instance   = Osmium::Javascript::Template::create_tags_instance(this);
                    js_object_instance = Osmium::Javascript::Template::create_node_instance(this);
                    js_geom_instance   = Osmium::Javascript::Template::create_node_geom_instance(this);
                }

                ~Wrapper() {
                    delete object;
                }

                Osmium::OSM::Node *get_object() {
                    return object;
                }

            }; // class Wrapper

        } // namespace Node

        namespace Way {

            class Wrapper : public Osmium::Javascript::Object::Wrapper {

                public:

                Osmium::OSM::Way *object;

                v8::Local<v8::Object> js_nodes_instance;
                v8::Local<v8::Object> js_geom_instance;

                Wrapper() : Osmium::Javascript::Object::Wrapper() {
                    object = new Osmium::OSM::Way;
                    object->wrapper = this;

                    js_tags_instance   = Osmium::Javascript::Template::create_tags_instance(this);
                    js_object_instance = Osmium::Javascript::Template::create_way_instance(this);
                    js_nodes_instance  = Osmium::Javascript::Template::create_way_nodes_instance(this);
                    js_geom_instance   = Osmium::Javascript::Template::create_way_geom_instance(this);
                }

                ~Wrapper() {
                    delete object;
                }

                Osmium::OSM::Way *get_object() {
                    return object;
                }

            }; // class Wrapper

        } // namespace Way

        namespace Relation {

            class Wrapper : public Osmium::Javascript::Object::Wrapper {

                public:

                Osmium::OSM::Relation *object;

                v8::Local<v8::Object> js_members_instance;

                Wrapper() : Osmium::Javascript::Object::Wrapper() {
                    object = new Osmium::OSM::Relation;
                    object->wrapper = this;

                    js_tags_instance    = Osmium::Javascript::Template::create_tags_instance(this);
                    js_object_instance  = Osmium::Javascript::Template::create_relation_instance(this);
                    js_members_instance = Osmium::Javascript::Template::create_relation_members_instance(this);
                }

                ~Wrapper() {
                    delete object;
                }

                Osmium::OSM::Relation *get_object() {
                    return object;
                }

            }; // class Wrapper

        } // namespace Relation

        namespace Multipolygon {

            class Wrapper : public Osmium::Javascript::Object::Wrapper {

                public:

                Osmium::OSM::Multipolygon *object;

                Wrapper() : Osmium::Javascript::Object::Wrapper() {
                    object = NULL;

                    js_tags_instance    = Osmium::Javascript::Template::create_tags_instance(this);
                    js_object_instance  = Osmium::Javascript::Template::create_multipolygon_instance(this);
                }

                Wrapper(Osmium::OSM::Multipolygon *mp) : Osmium::Javascript::Object::Wrapper() {
                    object = mp;
                    object->wrapper = this;

                    js_tags_instance    = Osmium::Javascript::Template::create_tags_instance(this);
                    js_object_instance  = Osmium::Javascript::Template::create_multipolygon_instance(this);
                }

                void set_object(Osmium::OSM::Multipolygon *o) {
                    //if (object) object->wrapper = NULL;
                    object = o;
                    object->wrapper = this;
                }

                Osmium::OSM::Multipolygon *get_object() {
                    return object;
                }

            }; // class Wrapper

        } // namespace Multipolygon

    } // namespace Javascript

} // namespace Osmium

#endif // OSMIUM_JAVASCRIPT_HPP
