// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/mcrl2_squadt_interface.h
/// \brief Interface for simplified connection and communication with SQuADT

#ifndef MCRL2_SPECIFIC_SQUADT_INTERFACE_H_
#define MCRL2_SPECIFIC_SQUADT_INTERFACE_H_

#include "mcrl2/core/messaging.h"

#include "mcrl2/utilities/squadt_interface.h"

namespace mcrl2 {
  namespace utilities {
    namespace squadt {

      /// \cond INTERNAL_DOCS
      void relay_message(const ::mcrl2::core::messageType, const char*);

      /**
       * \brief mCRL2 tool specific squadt interface
       *
       * In addition to the interface above, the message system used by most
       * tools in the mCRL2 toolset is initialised such that messages are
       * relayed.
       *
       * \note only works for one object at a time
       **/
      class mcrl2_tool_interface : public squadt::tool_interface {

        /// \brief relays messages from the mCRL2 messaging system
        friend void relay_message(const ::mcrl2::core::messageType, const char* data);

        private:

          /* send status message, used only to relay messages */
          static mcrl2_tool_interface*& get_reporter() {
            static mcrl2_tool_interface* reporter;

            return reporter;
          }

          using tool_interface::send_report;

          void send_report(const tipi::report::type t, std::string const& d) {
            squadt::tool_interface::send_report(t, d);
          }

          static bool initialise_enumerated_type_conversions() {
#ifdef __LIBREWRITE_H
            tipi::datatype::enumeration< RewriteStrategy > strategy_enumeration;

            strategy_enumeration.
              add(GS_REWR_INNER, "inner").
              add(GS_REWR_INNERC, "innerc").
              add(GS_REWR_JITTY, "jitty").
              add(GS_REWR_JITTYC, "jittyc");
#endif

#ifdef __LIBLTS_H
            tipi::datatype::enumeration< mcrl2::lts::lts_equivalence > transformation_methods;

            transformation_methods.
              add(mcrl2::lts::lts_eq_none, "none").
              add(mcrl2::lts::lts_eq_bisim, "modulo_strong_bisimulation").
              add(mcrl2::lts::lts_eq_branching_bisim, "modulo_branching_bisimulation").
              add(mcrl2::lts::lts_eq_sim, "strong_simulation_equivalence").
              add(mcrl2::lts::lts_eq_trace, "modulo_trace_equivalence").
              add(mcrl2::lts::lts_eq_weak_trace, "modulo_weak_trace_equivalence").
              add(mcrl2::lts::lts_eq_isomorph, "isomorphism");

            tipi::datatype::enumeration< mcrl2::lts::lts_type > storage_types;

            storage_types.
              add(mcrl2::lts::lts_none, "none").
              add(mcrl2::lts::lts_mcrl2, "mCRL2").
              add(mcrl2::lts::lts_aut, "Aldebaran").
              add(mcrl2::lts::lts_mcrl, "mCRL").
              add(mcrl2::lts::lts_svc, "SVC").
              add(mcrl2::lts::lts_fsm, "FSM").
#ifdef USE_BCG
              add(mcrl2::lts::lts_bcg, "BCG").
#endif
              add(mcrl2::lts::lts_dot, "DOT");
#endif

              return true;
          }

        protected:

          /** \brief initialisation after connection to SQuADt */
          mcrl2_tool_interface() {
            static bool initialised = initialise_enumerated_type_conversions();

            static_cast< void >(initialised);

            mcrl2_tool_interface::get_reporter() = this;

            mcrl2::core::gsSetCustomMessageHandler(relay_message);

            tipi::utility::logger::log_level l = tipi::utility::logger::get_default_filter_level();

            mcrl2::core::gsSetNormalMsg();

            if (2 < l) {
              mcrl2::core::gsSetVerboseMsg();

              if (3 < l) {
                mcrl2::core::gsSetDebugMsg();
              }
            }
          }

          /** \brief finalisation after termination signal has been received */
          virtual ~mcrl2_tool_interface() {
            /* Unregister message relay */
            mcrl2::core::gsSetCustomMessageHandler(0);
          }
      };

      /** \brief Used to relay messages generated using core::print */
      inline void relay_message(const ::mcrl2::core::messageType t, const char* data) {
        tipi::report::type report_type;

        switch (t) {
          case mcrl2::core::gs_notice:
            report_type = tipi::report::notice;
            break;
          case mcrl2::core::gs_warning:
            report_type = tipi::report::warning;
            break;
          case mcrl2::core::gs_error:
          default:
            report_type = tipi::report::error;
            break;
        }

        mcrl2_tool_interface::get_reporter()->send_report(report_type, std::string(data));
      }
      /// \endcond

#if defined(__WXWINDOWS__)
      /**
       * \brief mCRL2 tools using wxWidgets specific squadt interface
       *
       * In addition to the interface above, the message system used by most
       * tools in the mCRL2 toolset is initialised such that messages are
       * relayed.
       *
       * \note only works for one object at a time
       **/
      typedef basic_wx_tool_interface< mcrl2_tool_interface > mcrl2_wx_tool_interface;
#endif
    }
  }
}

#endif
