#pragma once
#include "liblvgl/lvgl.h"
#include "robodash/view.hpp"
#include <functional>
#include <string>
#include <vector>

namespace rd {

/**
 * An autonomous function selector
 *
 * A function selector for easily managing autonomous routines. If available, automatically saves
 * the current configuration to an SD card and loads it on the next run. Also supports displaying
 * images from the SD card.
 *
 * @image html selector.png
 */
class Selector {
  public:
	using RoutineFunction = std::function<void()>;

	struct Routine {
		std::string name;
		RoutineFunction function;
		std::string image = "";
	};

	/**
	 * Create autonomous selector
	 *
	 * @param name Name of the autonomous selector
	 * @param autons Vector of autonomous rotuines
	 */
	Selector(std::string name, std::vector<Routine> autons);

	/**
	 * Create autonomous selector
	 *
	 * @param autons Vector of autonomous rotuines
	 */
	Selector(std::vector<Routine> autons);

	/**
	 * Run selected auton
	 */
	void run_auton();

	/**
	 * Set this view to the active view
	 */
	void focus();

  private:
	rd::View view;

	lv_obj_t *select_cont;
	lv_obj_t *selected_label;
	lv_obj_t *selected_img;

	std::string name;
	std::vector<rd::Selector::Routine> routines;
	std::optional<rd::Selector::Routine> selected_routine;

	static void select_cb(lv_event_t *event);
};

} // namespace rd