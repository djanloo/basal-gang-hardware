.PHONY: generate profile clean

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

clean:
	@echo "Cleaning all.."
	@rm -f basal_gang/*.c
	@rm -f basal_gang/*.cpp
	@rm -f basal_gang/*.so
	@rm -f basal_gang/*.html
	@rm -R -f basal_gang/build
	@rm -R -f basal_gang/__pycache__
	@echo "Cleaned."