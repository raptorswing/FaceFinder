TEMPLATE = subdirs

SUBDIRS += 	Core \
			Tests \
			FaceFinder
			
Tests.depends += Core
FaceFinder.depends += Core