man3_MANS=lasinfo.man las2las.man lasmerge.man
dist_man3_MANS = lasinfo.man las2las.man lasmerge.man

lasinfo.man: ../lasinfo.txt
	python ../rst2man.py ../lasinfo.txt lasinfo.man

las2las.man: ../las2las.txt
	python ../rst2man.py ../las2las.txt las2las.man

lasmerge.man: ../lasmerge.txt
	python ../rst2man.py ../lasmerge.txt lasmerge.man
