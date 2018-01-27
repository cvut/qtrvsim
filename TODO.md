* Firm endianity support
* Full unprivileged instructions support
* Windows support
* On cache also allow calculate what time it would take with stalls.
* Add at least one memory mapped periphery (leds or console?)
* Unit tests for hazard unit
* When we are scrolling or content is updated in MemoryView the layout
  recalculation is very annoying. It reorders view and that way blinks.
* When MemoryView is resized we should ensure that we stay in middle even if we
  add new column.
* Optimize MemoryView (it calls layout size and do_layout a lot)
