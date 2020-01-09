#include <chrono>
#include <iostream>

#include <caf/all.hpp>
#include <caf/io/all.hpp>

using namespace std::chrono_literals;
using namespace caf;

using turn_atom = atom_constant<atom("turn")>;
using print_atom = atom_constant<atom("print")>;

using push_atom = atom_constant<atom("push")>;
using pull_atom = atom_constant<atom("pull")>;

using plotter_ne_atom = atom_constant<atom("plotter_ne")>;
using plotter_nw_atom = atom_constant<atom("plotter_nw")>;
using plotter_se_atom = atom_constant<atom("plotter_se")>;
using plotter_sw_atom = atom_constant<atom("plotter_sw")>;

using turntable_west_atom = atom_constant<atom("ttable_w")>;
using turntable_east_atom = atom_constant<atom("ttable_e")>;

using is_empty_atom = atom_constant<atom("is_empty")>;

enum class Position {
  North,
  West,
  South,
  East,
  NorthWest,
  SouthWest,
  SouthEast,
  NorthEast
};

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
  return ((plotter == plotter_nw_atom::value ||
           plotter == plotter_ne_atom::value) &&
          turntable_position == Position::North) ||
         ((plotter == plotter_sw_atom::value ||
           plotter == plotter_se_atom::value) &&
          turntable_position == Position::South);
}

behavior plotter(event_based_actor *self, char color, bool loaded) {

  std::string plotterName = std::string{"Plotter "} + color;

  message_handler basic_handler{
      // current state
      [=](get_atom) { return std::make_pair(color, loaded); },
  };

  // messages when loaded with paper
  message_handler loaded_handler{// push paper from plotter
                                 [=](push_atom) -> result<ok_atom> {
                                   aout(self) << plotterName << ": pushing!"
                                              << std::endl;

                                   self->become(plotter(self, color, false));
                                   return ok_atom::value;
                                 },

                                 // print
                                 [=](print_atom) -> result<ok_atom> {
                                   aout(self) << plotterName << ": printing!"
                                              << std::endl;
                                   return ok_atom::value;
                                 }};

  // messages when empty
  message_handler unloaded_handler{// pull paper into plotter
                                   [=](pull_atom) -> result<ok_atom> {
                                     aout(self) << plotterName << ": pulling!"
                                                << std::endl;
                                     self->become(plotter(self, color, true));

                                     return ok_atom::value;
                                   }};

  return loaded ? basic_handler.or_else(loaded_handler)
                : basic_handler.or_else(unloaded_handler);
}

behavior turntable(event_based_actor *self, Position position,
                   Position direction, bool loaded) {

  std::string turntable_name = "Turntable " + to_string(position);

  // messages at any state
  message_handler basic_handler{
      // turn the turntable
      [=](turn_atom) {
        aout(self) << turntable_name << ": direction: " << direction
                   << ", loaded: " << loaded << ", turning" << std::endl;

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
        aout(self) << turntable_name << ": returning state (" << direction
                   << ", " << loaded << ")" << std::endl;
        return std::make_pair(direction, loaded);
      }};

  // pull into turntable
  message_handler pull_handler{[=](pull_atom) -> result<ok_atom> {
    aout(self) << turntable_name << ", direction: " << direction
               << ", loaded: " << loaded << ", pulling" << std::endl;

    self->become(turntable(self, position, direction, true));

    return ok_atom::value;
  }};

  // push from turntable
  message_handler push_handler{

      [=](push_atom) -> result<ok_atom> {
        aout(self) << turntable_name << ", direction: " << direction
                   << ", loaded: " << loaded << ", pushing" << std::endl;

        self->become(turntable(self, position, direction, false));

        return ok_atom::value;
      }};

  bool facingPlotter =
      direction == Position::North || direction == Position::South;
  bool canPush = loaded && (facingPlotter || direction == Position::East);
  bool canPull = !loaded && (facingPlotter || direction == Position::West);

  return canPush ? basic_handler.or_else(push_handler)
                 : basic_handler.or_else(pull_handler);
}

behavior helper(event_based_actor *self) {
  return {
      // requests the status of a turntable and a plotter
      [=](get_atom, atom_value turntable, atom_value plotter) {
        auto turntable_actor =
            actor_cast<actor>(self->system().registry().get(turntable));
        auto plotter_actor =
            actor_cast<actor>(self->system().registry().get(plotter));

        auto response = self->make_response_promise<
            std::tuple<Position, bool, char, bool>>();

        // request turntable status
        self->request(turntable_actor, infinite, get_atom::value)
            .then([=](std::pair<Position, bool> turntable_status) mutable {
              // reqest plotter status
              self->request(plotter_actor, infinite, get_atom::value)
                  .then([=](std::pair<char, bool> plotter_status) mutable {
                    response.deliver(std::make_tuple(
                        turntable_status.first, turntable_status.second,
                        plotter_status.first, plotter_status.second));
                  });
            });

        self->request(actor_cast<actor>(self), infinite, get_atom::value);
      },

      // prints at a plotter if possible
      [=](print_atom, atom_value plotter) -> result<ok_atom> {
        aout(self) << "Would print on " << to_string(plotter) << std::endl;

        auto turntable = plotter_turntable(plotter);
        //        auto helper_actor = self->system().spawn(helper);

        auto response = self->make_response_promise<ok_atom>();

        // request turntable and plotter position and print only if can
        self->request(actor_cast<actor>(self), infinite, get_atom::value,
                      turntable, plotter)
            .then([=](std::tuple<Position, bool, char, bool> status) {
              auto [turntable_direction, turntable_loaded, plotter_color,
                    plotter_loaded] = status;

              aout(self) << "Turntable loaded: " << turntable_loaded
                         << std::endl;
              aout(self) << "Turntable direction: " << turntable_direction
                         << std::endl;

              aout(self) << "Plotter loaded: " << plotter_loaded << std::endl;
              aout(self) << "Plotter color: " << plotter_color << std::endl;

              if (turntable_loaded && !plotter_loaded &&
                  is_facing_plotter(turntable_direction, plotter)) {

                auto turntable_actor =
                    actor_cast<actor>(self->system().registry().get(turntable));
                auto plotter_actor =
                    actor_cast<actor>(self->system().registry().get(plotter));

                self->request(turntable_actor, infinite, push_atom::value)
                    .then([=](ok_atom) {
                      self->request(plotter_actor, infinite, pull_atom::value)
                          .then([=](ok_atom) {
                            self->request(plotter_actor, infinite,
                                          print_atom::value)
                                .then([=](ok_atom) mutable {
                                  response.deliver(ok_atom::value);
                                });
                          });
                    });
              } else {
                aout(self) << "Issued print in inconsistent state!"
                           << std::endl;
              }
            });

        return ok_atom::value;
      }};
}

// Planner for East or West turntable and plotters
behavior region_planner(event_based_actor *self, Position region, bool loaded) {
  auto plotter_north = region == Position::East
                           ? static_cast<atom_value>(plotter_ne_atom::value)
                           : static_cast<atom_value>(plotter_nw_atom::value);

  auto plotter_south = region == Position::East
                           ? static_cast<atom_value>(plotter_se_atom::value)
                           : static_cast<atom_value>(plotter_sw_atom::value);

  auto turntable = region == Position::East
                       ? static_cast<atom_value>(turntable_east_atom::value)
                       : static_cast<atom_value>(turntable_west_atom::value);

  auto plotter_north_actor =
      actor_cast<actor>(self->system().registry().get(plotter_north));
  auto plotter_south_actor =
      actor_cast<actor>(self->system().registry().get(plotter_south));
  auto turntable_actor =
      actor_cast<actor>(self->system().registry().get(turntable));

  message_handler basic_handler = {
      // returns whether the plotter north, plotter south, and turntable do
      // not have paper
      [=](is_empty_atom) { return !loaded; },

      // turn the turntable until the to_position is reached
      [=](turn_atom, Position to_position) -> result<ok_atom> {
        aout(self) << "Want to turn " << to_string(turntable) << " "
                   << to_position << std::endl;

        auto result = self->make_response_promise<ok_atom>();
        // ask the turntable what is its position
        self->request(turntable_actor, infinite, get_atom::value)
            .await([=](std::pair<Position, bool> turntable_status) mutable {
              auto [turntable_position, turntable_loaded] = turntable_status;

              aout(self) << "Current position: " << turntable_position
                         << std::endl;

              if (turntable_position == to_position)
                result.deliver(ok_atom::value);
              else {
                self->request(turntable_actor, infinite, turn_atom::value)
                    .then([=](ok_atom) mutable {
                      result.delegate(actor_cast<actor>(self), turn_atom::value,
                                      to_position);
                    });
              }
            });

        return result;
      }};

  message_handler loaded_handler = {
      [=](print_atom, std::string sequnce) {},
      [=](push_atom) -> result<ok_atom> {
        // rotate to east and push
        auto response = self->make_response_promise<ok_atom>();

        self->request(actor_cast<actor>(self), infinite, turn_atom::value,
                      Position::East)
            .then([=](ok_atom) mutable {
              self->become(region_planner(self, region, false));
              response.deliver(ok_atom::value);
            });

        return response;
      }};

  message_handler unloaded_handler = {[=](pull_atom) -> result<ok_atom> {
    auto response = self->make_response_promise<ok_atom>();

    // rotate to west and pull
    self->request(actor_cast<actor>(self), infinite, turn_atom::value,
                  Position::West)
        .then([=](ok_atom) {
          self->request(turntable_actor, infinite, pull_atom::value)
              .then([=](ok_atom) mutable {
                self->become(region_planner(self, region, true));
                response.deliver(ok_atom::value);
              });
        });

    return response;
  }};

  return loaded ? basic_handler.or_else(loaded_handler)
                : basic_handler.or_else(unloaded_handler);

  //        auto response = self->make_response_promise<bool>();

  //        // ask the north actor whether it is empty
  //        self->request(plotter_north_actor, infinite,
  //        get_atom::value)
  //            .await([=](std::pair<char, bool> north_actor_status)
  //            mutable {
  //              // north actor not empty: return the result
  //              if (north_actor_status.second) {
  //                response.deliver(false);
  //              } else {
  //                // north actor empty: ask the south actor and then
  //                the turntable self->request(plotter_south_actor,
  //                infinite, get_atom::value)
  //                    .await(
  //                        [=](std::pair<char, bool>
  //                        south_actor_status) mutable {
  //                          // south actor not empty: return the
  //                          result if (south_actor_status.second) {
  //                            response.deliver(false);
  //                          } else {
  //                            // south actor empty: ask the
  //                            turntable
  //                            self->request(turntable_actor,
  //                            infinite,
  //                                          get_atom::value)
  //                                .await([=](std::pair<Position,
  //                                bool>
  //                                               turntableStatus)
  //                                               mutable {
  //                                  // return whether the turntable
  //                                  is empty
  //                                  response.deliver(!turntableStatus.second);
  //                                });
  //                          }
  //                        });
  //              }
  //            }
}

behavior planner(event_based_actor *self, std::string plotters) {
  char plotter_nw = plotters[0];
  char plotter_sw = plotters[1];
  char plotter_se = plotters[2];
  char plotter_ne = plotters[3];

  message_handler print_handler = {[=](print_atom, std::string sequence) {
    aout(self) << "planner::print(" << sequence << std::endl;

    // accept new sequences only if both western plotters and the western
    // turntable do not have paper

    auto turntable_actor = actor_cast<actor>(
        self->system().registry().get(turntable_west_atom::value));
  }};

  return print_handler;
  //  return {[=](put_atom, std::string sequence, size_t current_pos) {
  //    aout(self) << "planner::put_atom: " << sequence << "[" << current_pos <<
  //    "]"
  //               << std::endl;

  //    char ch = sequence[current_pos];

  //    auto turntable_west_actor = actor_cast<actor>(
  //        self->system().registry().get(turntable_west_atom::value));
  //    auto turntable_east_actor = actor_cast<actor>(
  //        self->system().registry().get(turntable_east_atom::value));

  //    auto plotter_nw_actor = actor_cast<actor>(
  //        self->system().registry().get(plotter_nw_atom::value));
  //    auto plotter_sw_actor = actor_cast<actor>(
  //        self->system().registry().get(plotter_sw_atom::value));

  //    self->request(turntable_west_actor, 5s, get_atom::value)
  //        .then([=](std::pair<Position, bool> state) {
  //          auto [direction, loaded] = state;

  //          // if west actor has paper, rotate it to print position
  //          // if can't print, rotate it to push paper to the east turntable
  //          if (loaded) {
  //            if (ch == plotter_nw && direction == Position::North) {
  //              self->request(plotter_nw_actor, infinite, print_atom::value,
  //              ch)
  //                  .then([=]() {
  //                    self->send(self, infinite, put_atom::value, sequence,
  //                               current_pos + 1);
  //                  });
  //            } else if (ch == plotter_sw && direction == Position::South) {
  //              self->request(plotter_sw_actor, infinite, print_atom::value,
  //              ch)
  //                  .then([=]() {
  //                    self->send(self, infinite, put_atom::value, sequence,
  //                               current_pos + 1);
  //                  });
  //            } else if (ch != plotter_nw && ch != plotter_sw &&
  //                       direction == Position::East) {
  //              self->request(turntable_west_actor, infinite,
  //              push_atom::value)
  //                  .then([=]() {
  //                    self->send(self, infinite, put_atom::value, sequence,
  //                               current_pos);
  //                  });
  //            } else {
  //              self->request(turntable_west_actor, infinite,
  //              turn_atom::value)
  //                  .then([=]() {
  //                    self->send(self, infinite, put_atom::value, sequence,
  //                               current_pos);
  //                  });
  //            }
  //          }
  //        });
  //  }};
}

void caf_main(caf::actor_system &system) {
  auto planner_actor = system.spawn(planner, "CMYK");

  auto turntable_west_actor =
      system.spawn(turntable, Position::West, Position::North, false);
  auto turntable_east_actor =
      system.spawn(turntable, Position::East, Position::North, false);

  auto plotter_nw_actor = system.spawn(plotter, 'C', false);
  auto plotter_sw_actor = system.spawn(plotter, 'M', false);
  auto plotter_se_actor = system.spawn(plotter, 'Y', false);
  auto plotter_ne_actor = system.spawn(plotter, 'K', false);

  scoped_actor self{system};

  system.registry().put(turntable_west_atom::value, turntable_west_actor);
  system.registry().put(turntable_east_atom::value, turntable_east_actor);
  system.registry().put(plotter_nw_atom::value, plotter_nw_actor);
  system.registry().put(plotter_sw_atom::value, plotter_sw_actor);
  system.registry().put(plotter_se_atom::value, plotter_se_actor);
  system.registry().put(plotter_ne_atom::value, plotter_ne_actor);

  auto region_planner_actor =
      system.spawn(region_planner, Position::West, false);
  self->request(region_planner_actor, infinite, pull_atom::value)
      .receive([&](ok_atom) { std::cout << "Done!" << std::endl; },
               [&](error e) {
                 std::cout << "\nError occurred:" << system.render(e)
                           << std::endl;
               });

  //  self->request(planner_actor, infinite, print_atom::value,
  //                plotter_nw_atom::value)
  //      .receive([](ok_atom) { std::cout << "\nDone" << std::endl; },
  //               [&](error e) {
  //                 std::cout << "\nError occurred:" << system.render(e)
  //                           << std::endl;
  //               });
}

CAF_MAIN()
