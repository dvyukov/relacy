# User-customizable variables:
CXX ?= c++
CXXFLAGS ?=
DEPFLAGS ?= -MD -MF $(@).d -MP -MT $(@)
build_dir = build

.SECONDARY:

example_programs = cli_ws_deque
test_programs = ntest/ntest

example_exe_files = $(foreach name,$(example_programs),$(build_dir)/example/$(name)/$(name))
test_exe_files = $(foreach name,$(test_programs),$(build_dir)/test/$(name))

exe_files = $(example_exe_files) $(test_exe_files)
o_files = $(exe_files:=.cpp.o)

ansi_term_csi = [
ansi_term_bold = $(ansi_term_csi)1m
ansi_term_green = $(ansi_term_csi)32m
ansi_term_red = $(ansi_term_csi)31m
ansi_term_reset = $(ansi_term_csi)m

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) -c
LINK.cpp = $(CXX) $(CXXFLAGS)

.PHONY: all
all: examples tests

.PHONY: examples tests
examples: $(example_exe_files)
tests: $(test_exe_files)

.PHONY: check
check: check-examples check-tests

.PHONY: check-examples check-tests
check-examples: $(example_exe_files:=.check-result)
check-tests: $(test_exe_files:=.check-result)

$(build_dir)/%.check-result: $(build_dir)/% always-run
	@ \
	printf '%s%s ...%s\n' $(ansi_term_bold) $(*) $(ansi_term_reset) >&2; \
	$(<); \
	status="$${?}"; \
	printf %d "$${status}" >$(@); \
	if [ "$${status}" -eq 0 ]; then \
		printf '%s%s %s%s\n' $(ansi_term_green) $(*) OK $(ansi_term_reset); \
	else \
		printf '%s%s %s%s\n' $(ansi_term_red) $(*) FAIL $(ansi_term_reset); \
	fi >&2; \
	exit "$${status}"

$(build_dir)/%: $(build_dir)/%.cpp.o
	$(LINK.cpp) $(^) -o $(@)

$(build_dir)/%.cpp.o: %.cpp
	@mkdir -p $(dir $(@))
	$(COMPILE.cpp) -o $(@) $(<)

.PHONY: clean
clean:
	rm -fr -- $(build_dir)/

.PHONY: always-run
always-run:

-include $(o_files:=.d)
