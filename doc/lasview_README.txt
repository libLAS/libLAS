****************************************************************

  LASview : a super simple OpenGL-based viewer for LAS files

****************************************************************

example usage:

>> lasview lidar.las
>> lasview -i lidar.las

reads around 500000 subsampled lidar points and displays in 100 steps

>> lasview -i lidar.las -win 1600 1200

same as above but with a larger display window

>> lasview -i lidar.las -steps 10 -points 200000

reads around 200000 subsampled lidar points and displays in 10 steps

interactive options:

<space> switch between rotate/translate/zoom
<-/=>   render points smaller/bigger
<[/]>   exaggerate/mellow elevation differences
<c>     change color mode
<b>     hide/show bounding box
<s/S>   step forward/backward
<t/T>   tiny step forward/backward
<r>     out-of-core full resolution rendering

----

try these sources for sample lidar data in LAS format:

http://www.geoict.net/LidarData/Data/OptechSampleLAS.zip
http://www.qcoherent.com/data/LP360_Sample_Data.rar
http://www.appliedimagery.com/Serpent%20Mound%20Model%20LAS%20Data.las

if you find bugs let me (isenburg@cs.unc.edu) know.
