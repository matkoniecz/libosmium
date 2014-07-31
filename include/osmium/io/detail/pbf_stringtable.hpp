#ifndef OSMIUM_IO_DETAIL_PBF_STRINGTABLE_HPP
#define OSMIUM_IO_DETAIL_PBF_STRINGTABLE_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013,2014 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <algorithm>
#include <iterator>
#include <map>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include <osmpbf/osmpbf.h>

namespace osmium {

    namespace io {

        namespace detail {

            /**
             * StringTable management for PBF writer
             *
             * All strings are stored as indexes to rows in a StringTable. The StringTable contains
             * one row for each used string, so strings that are used multiple times need to be
             * stored only once. The StringTable is sorted by usage-count, so the most often used
             * string is stored at index 1.
             */
            class StringTable {

            public:

                /// type for string IDs (interim and final)
                typedef uint16_t string_id_type;

            private:

                /**
                 * this is the struct used to build the StringTable. It is stored as
                 * the value-part in the strings-map.
                 *
                 * when a new string is added to the map, its count is set to 0 and
                 * the interim_id is set to the current size of the map. This interim_id
                 * is then stored into the pbf-objects.
                 *
                 * before the PrimitiveBlock is serialized, the map is sorted by count
                 * and stored into the pbf-StringTable. Afterwards the interim-ids are
                 * mapped to the "real" id in the StringTable.
                 *
                 * this way often used strings get lower ids in the StringTable. As the
                 * protobuf-serializer stores numbers in variable bit-lengths, lower
                 * IDs means less used space in the resulting file.
                 */
                struct string_info {
                    /// number of occurrences of this string
                    uint16_t count;

                    /// an intermediate-id
                    string_id_type interim_id;
                };

                /**
                 * Interim StringTable, storing all strings that should be written to
                 * the StringTable once the block is written to disk.
                 */
                typedef std::map<std::string, string_info> string2string_info_type;
                string2string_info_type m_strings {};

                /**
                 * This vector is used to map the interim IDs to real StringTable IDs after
                 * writing all strings to the StringTable.
                 */
                typedef std::vector<string_id_type> interim_id2id_type;
                interim_id2id_type m_id2id_map {};

                size_t m_size = 0;

            public:

                StringTable() {
                }

                friend bool operator<(const string_info& lhs, const string_info& rhs) {
                    return lhs.count > rhs.count;
                }

                /**
                 * record a string in the interim StringTable if it's missing, otherwise just increase its counter,
                 * return the interim-id assigned to the string.
                 */
                string_id_type record_string(const std::string& string) {
                    string_info& info = m_strings[string];
                    if (info.interim_id == 0) {
                        ++m_size;
                        assert(m_size < std::numeric_limits<string_id_type>::max());
                        info.interim_id = static_cast<string_id_type>(m_size);
                    } else {
                        info.count++;
                    }
                    return info.interim_id;
                }

                /**
                 * Sort the interim StringTable and store it to the real protobuf StringTable.
                 * while storing to the real table, this function fills the id2id_map with
                 * pairs, mapping the interim-ids to final and real StringTable ids.
                 *
                 * Note that the m_strings table is a std::map and as such is sorted lexicographically.
                 * When the transformation into the sortedby multimap is done, it gets sorted by
                 * the count. The end result (at least with the glibc standard container/algorithm
                 * implementation) is that the string table is sorted first by reverse count (ie descending)
                 * and then by reverse lexicographic order.
                 */
                void store_stringtable(OSMPBF::StringTable* st) {
                    // add empty StringTable entry at index 0
                    // StringTable index 0 is reserved as delimiter in the densenodes key/value list
                    // this line also ensures that there's always a valid StringTable
                    st->add_s("");

                    std::multimap<string_info, std::string> sortedbycount;

                    m_id2id_map.resize(m_size+1);

                    std::transform(m_strings.begin(), m_strings.end(),
                                std::inserter(sortedbycount, sortedbycount.begin()),
                                [](const std::pair<std::string, string_info>& p) {
                                        return std::pair<string_info, std::string>(p.second, p.first);
                                });

                    string_id_type n=0;

                    for (const auto& mapping : sortedbycount) {
                        // add the string of the current item to the pbf StringTable
                        st->add_s(mapping.second);

                        // store the mapping from the interim-id to the real id
                        m_id2id_map[mapping.first.interim_id] = ++n;
                    }
                }

                /**
                 * Map from an interim ID to a real string ID.
                 */
                string_id_type map_string_id(const string_id_type interim_id) const {
                    return m_id2id_map[interim_id];
                }

                /**
                 * Clear the stringtable, preparing for the next block.
                 */
                void clear() {
                    m_strings.clear();
                    m_id2id_map.clear();
                    m_size = 0;
                }

            }; // class StringTable

        } // namespace detail

    } // namespace io

} // namespace osmium

#endif // OSMIUM_IO_DETAIL_PBF_STRINGTABLE_HPP
