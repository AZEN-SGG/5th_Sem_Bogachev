TASKS = 01Ex 02Ex 03Ex 04Ex 05Ex 06Ex 07Ex 08Ex 09Ex

.PHONY: all clean test $(TASKS)
all: $(TASKS)

$(TASKS):
	$(MAKE) -C $@

clean:
	@for task in $(TASKS); do $(MAKE) -C $$task clean || exit $$?; done

test: all
	python3 tests/test_runner.py
