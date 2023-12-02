.PHONY: generate profile remake clean

generate:
	@python3 -m basal_gang.setup

profile:
	# @make clean
	@python3 -m basal_gang.setup --profile

notrace:
	# @make clean
	@python3 -m basal_gang.setup --notrace

hardcore:
	# make clean
	@python3 -m basal_gang.setup --hardcore

hardcoreprofile:
	# make clean
	@python3 -m basal_gang.setup --hardcore --profile

remake:
	@python3 -m basal_gang.setup --remake

debug:
	@ python3 -m basal_gang.setup --debug

clean:
	@echo "Cleaning all.."
	# @rm -f basal_gang/*.c
	# @rm -f basal_gang/*.cpp
	@rm -f basal_gang/*.so
	@rm -f basal_gang/bin/*.so
	@rm -f basal_gang/*.html
	@rm -R -f basal_gang/build
	@rm -R -f basal_gang/__pycache__
	@echo "Cleaned."