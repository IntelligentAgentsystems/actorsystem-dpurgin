#include "agentsystem.h"
#include "agentsystemconnector.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <caf/all.hpp>
#include <caf/io/all.hpp>

#include <QThread>

using namespace std::chrono_literals;
using namespace caf;

// actors
using planner_atom = atom_constant<atom("planner")>;
using connector_bridge_atom = atom_constant<atom("connector")>;

using plotter_ne_atom = atom_constant<atom("plotter_ne")>;
using plotter_nw_atom = atom_constant<atom("plotter_nw")>;
using plotter_se_atom = atom_constant<atom("plotter_se")>;
using plotter_sw_atom = atom_constant<atom("plotter_sw")>;

using turntable_west_atom = atom_constant<atom("ttable_w")>;
using turntable_east_atom = atom_constant<atom("ttable_e")>;

// messages
using turn_atom = atom_constant<atom("turn")>;
using print_atom = atom_constant<atom("print")>;

using push_atom = atom_constant<atom("push")>;
using pull_atom = atom_constant<atom("pull")>;

using is_empty_atom = atom_constant<atom("is_empty")>;

using set_turntable_direction_atom = atom_constant<atom("set_tt_dir")>;
using set_plotter_props_atom = atom_constant<atom("set_pprops")>;

std::string to_string(Position direction) {
  switch (direction) {
  case Position::North:
    return "North";
  case Position::West:
    return "West";
  case Position::South:
    return "South";
  case Position::East:
    return "East";
  case Position::NorthWest:
    return "NorthWest";
  case Position::SouthWest:
    return "SouthWest";
  case Position::SouthEast:
    return "SouthEast";
  case Position::NorthEast:
    return "NorhtEast";
  }

  return "";
}

atom_value position_to_turntable(Position position) {
  switch (position) {
  case Position::West:
  case Position::NorthWest:
  case Position::SouthWest:
    return turntable_west_atom::value;

  case Position::East:
  case Position::NorthEast:
  case Position::SouthEast:
    return turntable_east_atom::value;

  default:
    CAF_ASSERT(false);
  }
}

atom_value position_to_plotter(Position position) {
  switch (position) {
  case Position::NorthWest:
    return plotter_nw_atom::value;

  case Position::NorthEast:
    return plotter_ne_atom::value;

  case Position::SouthWest:
    return plotter_sw_atom::value;

  case Position::SouthEast:
    return plotter_se_atom::value;

  default:
    CAF_ASSERT(false);
  }
}

atom_value plotter_turntable(atom_value plotter) {
  if (plotter == plotter_nw_atom::value || plotter == plotter_sw_atom::value)
    return turntable_west_atom::value;
  else
    return turntable_east_atom::value;
}

bool is_facing_plotter(Position turntable_position, atom_value plotter) {
  return ((plotter == plotter_nw_atom::value || plotter == plotter_ne_atom::value) &&
          turntable_position == Position::North) ||
         ((plotter == plotter_sw_atom::value || plotter == plotter_se_atom::value) &&
          turntable_position == Position::South);
}

behavior connector_bridge(event_based_actor *self, AgentSystemConnector *connector) {
  auto planner = actor_cast<actor>(self->system().registry().get(planner_atom::value));
  connector->installPrintCallback([=](std::string sequence) {
    aout(self) << "connector_bridge: received print " << sequence << std::endl;
    self->request(planner, infinite, print_atom::value, sequence);
  });

  return {[=](set_turntable_direction_atom, Position turntable_position, Position turntable_direction) {
            if (turntable_position == Position::East)
              connector->setTurntableEastDirection(turntable_direction);
            else if (turntable_position == Position::West)
              connector->setTurntableWestDirection(turntable_direction);
          },
          [=](set_plotter_props_atom, Position plotter_position, char color, bool loaded) {
            if (plotter_position == Position::NorthEast) {
              connector->setPlotterNorthEastColor(color);
            } else if (plotter_position == Position::NorthWest) {
              connector->setPlotterNorthWestColor(color);
            } else if (plotter_position == Position::SouthEast) {
              connector->setPlotterSouthEastColor(color);
            } else if (plotter_position == Position::SouthWest) {
              connector->setPlotterSouthWestColor(color);
            }
          }};
}

behavior plotter(event_based_actor *self, Position position, char color, bool loaded) {

  std::string plotterName = std::string{"Plotter "} + color;

  auto connector_bridge_actor = actor_cast<actor>(self->system().registry().get(connector_bridge_atom::value));
  self->send(connector_bridge_actor, set_plotter_props_atom::value, position, color, loaded);

  message_handler basic_handler{
      // current state
      [=](get_atom) { return std::make_pair(color, loaded); },
  };

  // messages when loaded with paper
  message_handler loaded_handler{// push paper from plotter
                                 [=](push_atom) -> result<ok_atom> {
                                   aout(self) << plotterName << ": pushing!" << std::endl;

                                   QThread::sleep(1);

                                   self->become(plotter(self, position, color, false));
                                   return ok_atom::value;
                                 },

                                 // print
                                 [=](print_atom) -> result<ok_atom> {
                                   aout(self) << plotterName << ": printing!" << std::endl;
                                   return ok_atom::value;
                                 }};

  // messages when empty
  message_handler unloaded_handler{// pull paper into plotter
                                   [=](pull_atom) -> result<ok_atom> {
                                     aout(self) << plotterName << ": pulling!" << std::endl;
                                     self->become(plotter(self, position, color, true));

                                     return ok_atom::value;
                                   }};

  return loaded ? basic_handler.or_else(loaded_handler) : basic_handler.or_else(unloaded_handler);
}

behavior turntable(event_based_actor *self, Position position, Position direction, bool loaded) {

  std::string turntable_name = "Turntable " + to_string(position);

  auto connector_bridge_actor = actor_cast<actor>(self->system().registry().get(connector_bridge_atom::value));
  self->send(connector_bridge_actor, set_turntable_direction_atom::value, position, direction);

  // messages at any state
  message_handler basic_handler{// turn the turntable
                                [=](turn_atom) {
                                  aout(self) << turntable_name << ": direction: " << direction << ", loaded: " << loaded
                                             << ", turning" << std::endl;

                                  QThread::sleep(1);

                                  switch (direction) {
                                  case Position::North:                                    
                                    self->become(turntable(self, position, Position::West, loaded));
                                    break;

                                  case Position::West:
                                    self->become(turntable(self, position, Position::South, loaded));
                                    break;

                                  case Position::South:
                                    self->become(turntable(self, position, Position::East, loaded));
                                    break;

                                  case Position::East:
                                    self->become(turntable(self, position, Position::North, loaded));
                                    break;
                                  }

                                  return ok_atom::value;
                                },

                                // current state
                                [=](get_atom) {
                                  aout(self) << turntable_name << ": returning state (" << direction << ", " << loaded
                                             << ")" << std::endl;
                                  return std::make_pair(direction, loaded);
                                }};

  // pull into turntable
  message_handler pull_handler{[=](pull_atom) -> result<ok_atom> {
    aout(self) << turntable_name << ", direction: " << direction << ", loaded: " << loaded << ", pulling" << std::endl;

    QThread::sleep(1);
    self->become(turntable(self, position, direction, true));

    return ok_atom::value;
  }};

  // push from turntable
  message_handler push_handler{

      [=](push_atom) -> result<ok_atom> {
        aout(self) << turntable_name << ", direction: " << direction << ", loaded: " << loaded << ", pushing"
                   << std::endl;

        QThread::sleep(1);

        self->become(turntable(self, position, direction, false));

        return ok_atom::value;
      }};

  bool facingPlotter = direction == Position::North || direction == Position::South;
  bool canPush = loaded && (facingPlotter || direction == Position::East);
  bool canPull = !loaded && (facingPlotter || direction == Position::West);

  return canPush ? basic_handler.or_else(push_handler) : basic_handler.or_else(pull_handler);
}

behavior helper(event_based_actor *self) {
  std::string name = "helper";

  return {// requests the status of a turntable and a plotter
          [=](get_atom, atom_value turntable, atom_value plotter) {
            auto turntable_actor = actor_cast<actor>(self->system().registry().get(turntable));
            auto plotter_actor = actor_cast<actor>(self->system().registry().get(plotter));

            auto response = self->make_response_promise<std::tuple<Position, bool, char, bool>>();

            // request turntable status
            self->request(turntable_actor, infinite, get_atom::value)
                .then([=](std::pair<Position, bool> turntable_status) mutable {
                  // reqest plotter status
                  self->request(plotter_actor, infinite, get_atom::value)
                      .then([=](std::pair<char, bool> plotter_status) mutable {
                        response.deliver(std::make_tuple(turntable_status.first, turntable_status.second,
                                                         plotter_status.first, plotter_status.second));
                      });
                });

            return response;
          },

          // prints at a plotter if possible
          [=](print_atom, atom_value plotter) -> result<ok_atom> {
            aout(self) << name << ": Would print on " << to_string(plotter) << std::endl;

            auto turntable = plotter_turntable(plotter);

            auto response = self->make_response_promise<ok_atom>();

            // request turntable and plotter position and print only if can
            self->request(actor_cast<actor>(self), infinite, get_atom::value, turntable, plotter)
                .then([=](std::tuple<Position, bool, char, bool> status) {
                  auto [turntable_direction, turntable_loaded, plotter_color, plotter_loaded] = status;

                  aout(self) << name << ": Turntable loaded: " << turntable_loaded << std::endl;
                  aout(self) << name << ": Turntable direction: " << turntable_direction << std::endl;

                  aout(self) << name << ": Plotter loaded: " << plotter_loaded << std::endl;
                  aout(self) << name << ": Plotter color: " << plotter_color << std::endl;

                  if (turntable_loaded && !plotter_loaded && is_facing_plotter(turntable_direction, plotter)) {

                    auto turntable_actor = actor_cast<actor>(self->system().registry().get(turntable));
                    auto plotter_actor = actor_cast<actor>(self->system().registry().get(plotter));

                    // Push paper from turntable to plotter
                    self->request(turntable_actor, infinite, push_atom::value).then([=](ok_atom) {
                      // Pull paper from turntable to plotter
                      self->request(plotter_actor, infinite, pull_atom::value).then([=](ok_atom) {
                        // Print on plotter
                        self->request(plotter_actor, infinite, print_atom::value).then([=](ok_atom) {
                          // push paper from plotter to turntable
                          self->request(plotter_actor, infinite, push_atom::value).then([=](ok_atom) {
                            // Pull paper from plotter into turntable
                            self->request(turntable_actor, infinite, pull_atom::value).then([=](ok_atom) mutable {
                              // all done
                              aout(self) << name << ": done printing" << std::endl;
                              response.deliver(ok_atom::value);
                            });
                          });
                        });
                      });
                    });
                  } else {
                    aout(self) << name << ": Issued print in inconsistent state!" << std::endl;
                  }
                });

            return response;
          }};
}

// Planner for East or West turntable and plotters
behavior region_planner(event_based_actor *self, Position region, bool loaded, char north_color, char south_color) {
  auto plotter_north = region == Position::East ? static_cast<atom_value>(plotter_ne_atom::value)
                                                : static_cast<atom_value>(plotter_nw_atom::value);

  auto plotter_south = region == Position::East ? static_cast<atom_value>(plotter_se_atom::value)
                                                : static_cast<atom_value>(plotter_sw_atom::value);

  auto turntable = region == Position::East ? static_cast<atom_value>(turntable_east_atom::value)
                                            : static_cast<atom_value>(turntable_west_atom::value);

  auto plotter_north_actor = actor_cast<actor>(self->system().registry().get(plotter_north));
  auto plotter_south_actor = actor_cast<actor>(self->system().registry().get(plotter_south));
  auto turntable_actor = actor_cast<actor>(self->system().registry().get(turntable));

  std::string name = "Region Planner " + to_string(region);

  message_handler basic_handler = {
      // returns whether the plotter north, plotter south, and turntable do
      // not have paper
      [=](is_empty_atom) { return !loaded; },

      // turn the turntable until the to_position is reached
      [=](turn_atom, Position to_position) -> result<ok_atom> {
        aout(self) << name << ": Want to turn " << to_string(turntable) << " " << to_position << std::endl;

        auto result = self->make_response_promise<ok_atom>();
        // ask the turntable what is its position
        self->request(turntable_actor, infinite, get_atom::value)
            .await([=](std::pair<Position, bool> turntable_status) mutable {
              auto [turntable_position, turntable_loaded] = turntable_status;

              aout(self) << name << ": Current position: " << turntable_position << std::endl;

              if (turntable_position == to_position)
                result.deliver(ok_atom::value);
              else {
                self->request(turntable_actor, infinite, turn_atom::value).then([=](ok_atom) mutable {
                  result.delegate(actor_cast<actor>(self), turn_atom::value, to_position);
                });
              }
            });

        return result;
      }};

  message_handler loaded_handler = {
      [=](print_atom, char ch) -> result<ok_atom> {
        aout(self) << name << ": Want to print: " << ch << std::endl;

        auto helper_actor = self->system().spawn(helper);
        auto response = self->make_response_promise<ok_atom>();

        if (ch == north_color) {
          // turn turntable until it's in the north position
          self->request(actor_cast<actor>(self), infinite, turn_atom::value, Position::North)
              .then([=](ok_atom) mutable {
                // print on plotter
                response.delegate(helper_actor, print_atom::value, plotter_north);
              });
        } else {
          // turn turntable until it's in the south position
          self->request(actor_cast<actor>(self), infinite, turn_atom::value, Position::South)
              .then([=](ok_atom) mutable {
                // print on south plotter
                response.delegate(helper_actor, print_atom::value, plotter_south);
              });
        }
        return response;
      },
      [=](print_atom, std::string sequence) -> result<std::string> {
        aout(self) << name << ": Want to print sequence: " << sequence << std::endl;

        auto response = self->make_response_promise<std::string>();

        auto it = std::find_if(std::begin(sequence), std::end(sequence),
                               [north_color, south_color](char ch) { return ch == north_color || ch == south_color; });

        if (it == std::end(sequence)) {
          aout(self) << name << ": No printable colors: " << sequence;
          response.deliver(sequence);
        } else {
          char ch = *it;
          sequence.erase(it);
          self->request(actor_cast<actor>(self), infinite, print_atom::value, ch).then([=](ok_atom) mutable {
            response.delegate(actor_cast<actor>(self), print_atom::value, sequence);
          });
        }
        return response;
      },
      [=](push_atom) -> result<ok_atom> {
        // rotate to east and push
        auto response = self->make_response_promise<ok_atom>();

        self->request(actor_cast<actor>(self), infinite, turn_atom::value, Position::East).then([=](ok_atom) mutable {
          self->become(region_planner(self, region, false, north_color, south_color));
          response.deliver(ok_atom::value);
        });

        return response;
      }};

  message_handler unloaded_handler = {[=](pull_atom) -> result<ok_atom> {
    aout(self) << name << ": Pulling paper" << std::endl;

    auto response = self->make_response_promise<ok_atom>();

    // rotate to west and pull
    self->request(actor_cast<actor>(self), infinite, turn_atom::value, Position::West).then([=](ok_atom) {
      self->request(turntable_actor, infinite, pull_atom::value).then([=](ok_atom) mutable {
        self->become(region_planner(self, region, true, north_color, south_color));
        response.deliver(ok_atom::value);
        aout(self) << name << ": Paper pulled" << std::endl;
      });
    });

    return response;
  }};

  return loaded ? basic_handler.or_else(loaded_handler) : basic_handler.or_else(unloaded_handler);
}

behavior planner(event_based_actor *self, std::string plotters) {
  std::string name = "Planner";

  char plotter_nw = plotters[0];
  char plotter_sw = plotters[1];
  char plotter_se = plotters[2];
  char plotter_ne = plotters[3];

  auto west_planner = self->system().spawn(region_planner, Position::West, false, plotter_nw, plotter_sw);
  auto east_planner = self->system().spawn(region_planner, Position::East, false, plotter_ne, plotter_se);

  message_handler print_handler = {[=](print_atom, std::string sequence) {
    aout(self) << name << ": Printing " << sequence << std::endl;

    auto response = self->make_response_promise<ok_atom>();

    // accept new sequences only if west planner does not have paper
    self->request(west_planner, infinite, is_empty_atom::value).then([=](bool is_empty) {
      if (is_empty) {
        // pull paper to west planner
        self->request(west_planner, infinite, pull_atom::value).then([=](ok_atom) {
          // print on the west planner all printable characters
          self->request(west_planner, infinite, print_atom::value, sequence).then([=](std::string rest) {
            // after printing is complete, push the paper to the east turntable
            self->request(west_planner, infinite, push_atom::value).then([=](ok_atom) {
              // pull the paper from the west turntable into the east turntable
              self->request(east_planner, infinite, pull_atom::value).then([=](ok_atom) {
                // print on the east planner all printable characters
                self->request(east_planner, infinite, print_atom::value, rest)
                    .then([=](std::string /*shouldBeEmpty*/) mutable {
                      // push the paper from the east planner
                      response.delegate(east_planner, push_atom::value);
                    });
              });
            });
          });
        });
      }
    });

    return response;
  }};

  return print_handler;
}

void caf_main(caf::actor_system &system, AgentSystemConnector *connector) {
  auto connector_bridge_actor = system.spawn(connector_bridge, connector);

  auto planner_actor = system.spawn(planner, "CMYK");

  auto turntable_west_actor = system.spawn(turntable, Position::West, Position::North, false);
  auto turntable_east_actor = system.spawn(turntable, Position::East, Position::North, false);

  auto plotter_nw_actor = system.spawn(plotter, Position::NorthWest, 'C', false);
  auto plotter_sw_actor = system.spawn(plotter, Position::SouthWest, 'M', false);
  auto plotter_se_actor = system.spawn(plotter, Position::SouthEast, 'Y', false);
  auto plotter_ne_actor = system.spawn(plotter, Position::NorthEast, 'K', false);

  scoped_actor self{system};

  system.registry().put(planner_atom::value, planner_actor);
  system.registry().put(connector_bridge_atom::value, connector_bridge_actor);
  system.registry().put(turntable_west_atom::value, turntable_west_actor);
  system.registry().put(turntable_east_atom::value, turntable_east_actor);
  system.registry().put(plotter_nw_atom::value, plotter_nw_actor);
  system.registry().put(plotter_sw_atom::value, plotter_sw_actor);
  system.registry().put(plotter_se_atom::value, plotter_se_actor);
  system.registry().put(plotter_ne_atom::value, plotter_ne_actor);
}

void run_agent_system(int argc, char *argv[], AgentSystemConnector *connector) {
  actor_system_config cfg;
  // read CLI options
  cfg.parse(argc, argv);
  // create actor system and call caf_main
  actor_system system{cfg};
  caf_main(system, connector);
}
