#ifndef OSMIUM_AREA_PROBLEM_REPORTER_HPP
#define OSMIUM_AREA_PROBLEM_REPORTER_HPP

/*

This file is part of Osmium (http://osmcode.org/osmium).

Copyright 2014 Jochen Topf <jochen@topf.org> and others (see README).

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

#include <osmium/osm/location.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/osm/way.hpp>

namespace osmium {

    namespace area {

        class ProblemReporter {

        public:

            ProblemReporter() = default;
            
            virtual ~ProblemReporter() = default;

            /**
             * Report a duplicate node, ie. two nodes with the same location.
             *
             * @param node_id1       ID of the first node.
             * @param node_id2       ID of the second node.
             * @param location       Location of both nodes.
             */
            virtual void report_duplicate_node(osmium::object_id_type /*node_id1*/, osmium::object_id_type /*node_id2*/, osmium::Location /*location*/) {
            }

            /**
             * Report an intersection between two segments.
             *
             * @param object_id      ID of the way or multipolygon relation that is currently being assembled.
             * @param way1_id        ID of the first involved way.
             * @param way1_seg_start Location where the segment of the first way with the intersection starts
             * @param way1_seg_end   Location where the segment of the first way with the intersection ends
             * @param way2_id        ID of the second involved way.
             * @param way2_seg_start Location where the segment of the second way with the intersection starts
             * @param way2_seg_end   Location where the segment of the second way with the intersection ends
             * @param location       Location of the intersection. This might be slightly off the correct location due to rounding.
             */
            virtual void report_intersection(osmium::object_id_type /*object_id*/, osmium::object_id_type /*way1_id*/, osmium::Location /*way1_seg_start*/, osmium::Location /*way1_seg_end*/,
                                                                                   osmium::object_id_type /*way2_id*/, osmium::Location /*way2_seg_start*/, osmium::Location /*way2_seg_end*/, osmium::Location /*intersection*/) {
            }

            /**
             * Report an open ring.
             *
             * @param object_id      ID of the way or multipolygon relation that is currently being assembled.
             * @param end1           Location of the first open end.
             * @param end2           Location of the second open end.
             */
            virtual void report_ring_not_closed(osmium::object_id_type /*object_id*/, osmium::Location /*end1*/, osmium::Location /*end2*/) {
            }
           
            /**
             * Report a segment that should have role "outer", but has a different role.
             *
             * @param object_id      ID of the way or multipolygon relation that is currently being assembled.
             * @param way_id         ID of the way this segment is in.
             * @param seg_start      Start of the segment with the wrong role.
             * @param seg_end        End of the segment with the wrong role.
             */ 
            virtual void report_role_should_be_outer(osmium::object_id_type /*object_id*/, osmium::object_id_type /*way_id*/, osmium::Location /*seg_start*/, osmium::Location /*seg_end*/) {
            }

            /**
             * Report a segment that should have role "inner", but has a different role.
             *
             * @param object_id      ID of the way or multipolygon relation that is currently being assembled.
             * @param way_id         ID of the way this segment is in.
             * @param seg_start      Start of the segment with the wrong role.
             * @param seg_end        End of the segment with the wrong role.
             */ 
            virtual void report_role_should_be_inner(osmium::object_id_type /*object_id*/, osmium::object_id_type /*way_id*/, osmium::Location /*seg_start*/, osmium::Location /*seg_end*/) {
            }

        }; // class ProblemReporter

    } // namespace area

} // namespace osmium

#endif // OSMIUM_AREA_PROBLEM_REPORTER_HPP
