call version.bat
rem set ver=%ver1%%ver2%%ver3%
set tag_name=Genie_release-v%VERSION%
set svn_base_url_name=http://dev.siteview.com/svn/svnHDM
set svn_tag_url_name=http://dev.siteview.com/svn/svnHDM/Genie/1.1/tags
rem set svn_exe=Y:\Subversion\bin\svn.exe
set svn_exe=y:\svn-win32-1.6.6\bin\svn.exe
rem set base_work_dir=X:\oregon_trail_2\sonyericsson\X1\trunk
set base_work_dir=%CD%
set conf_file=tag_conf.ini

rem		y:\Python23\python.exe tag.py -m dry -b %svn_base_url_name% -e %svn_exe% %base_work_dir% > %conf_file%


y:\Python23\python tag.py -m cfg -b %svn_base_url_name% -d %svn_tag_url_name% -c %conf_file% -e %svn_exe% -t %base_work_dir% %tag_name%> tag_ext.cmd
	
	


pause

