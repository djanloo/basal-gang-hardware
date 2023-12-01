.PHONY: generate profile clean

generate:
	@python3 -m dummy_pkg.setup

profile:
	# @make clean
	@python3 -m dummy_pkg.setup --profile

notrace:
	# @make clean
	@python3 -m dummy_pkg.setup --notrace

hardcore:
	# make clean
	@python3 -m dummy_pkg.setup --hardcore

hardcoreprofile:
	# make clean
	@python3 -m dummy_pkg.setup --hardcore --profile

clean:
	@echo "Cleaning all.."
	@rm -f dummy_pkg/*.c
	@rm -f dummy_pkg/*.cpp
	@rm -f dummy_pkg/*.so
	@rm -f dummy_pkg/*.html
	@rm -R -f dummy_pkg/build
	@rm -R -f dummy_pkg/__pycache__
	@echo "Cleaned."