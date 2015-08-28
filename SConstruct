#=============================================================================
#
#       Description  :
#        
#
#------------------------------------------------------------------------------
#       History  :
#
#=============================================================================
import os

print "\n\t\t--->building Orbit Simulation Tool\n"

# release version
ReleaseVersion='OrbSim_v1.0.0'
#   set the programme name
ExeName='DmpOrbSim'

#Command(os.environ, os.environ['DMPSWSYS']+'/../Event/OrbitEvent/include/DmpEvtOrbit.h', 
#[Delete("$TARGET"),
#Copy("$TARGET","$SOURCE")]
#)


prefix=os.environ['DMPSWWORK']


## simple envBase ( no need to use envBase = Environment(ENV=os.environ)
envBase = Environment()  # Initialize the environment



##ROOT
rootsys=os.environ['ROOTSYS']
envBase.PrependENVPath('PATH', rootsys+'/bin')
envBase.ParseConfig("root-config --cflags --libs")


#envBase.Append(LIBPATH =os.environ['XERCESCROOT']+"/lib" )
#envBase.Append(LIBS ="libxerces-c-3.1")

##GEANT4
#g4sys=os.environ['G4INSTALL']
#envBase.PrependENVPath('PATH', g4sys+'/../../../bin')
#envBase.ParseConfig("geant4-config --cflags --libs")
#envBase.Prepend(CPPFLAGS = '-DG4VIS_USE')       # set our -DRelease or -DDebug
#envBase.Append(CPPFLAGS = ' -DG4UI_USE')
#envBase.Append(CPPFLAGS = ' -DG4VIS_USE_OPENGLX')


#subDetectors=['Psd','Stk','Bgo','Nud']
#SConscript(prefix+'/../Event/Event.scons',exports=['envBase','prefix','subDetectors'])


envBase.Append(CPPPATH = prefix+"/include")
#envBase.Append(CPPPATH = prefix+"/../Event/OrbitEvent/include")
envBase.Append(LIBPATH =prefix+"/lib")
#envBase.Append(LIBPATH =os.environ['G4INCLUDE'] )
envBase.Append(LIBS ="libDmpMyEvt" )


print 'ENV = ', envBase['ENV']
print 'CC = ', envBase['CC']
print 'CFLAGS = ', envBase['CFLAGS']
print 'CPPPATH = ', envBase['CPPPATH']
print 'LIBPATH = ', envBase['LIBPATH']
print 'LIBS = ', envBase['LIBS']


sources_cxx = Glob('src/*.cxx')
sources_c = Glob('src/*.c')
sources = [sources_cxx,sources_c]
execute = envBase.Program(target = ExeName, source = sources)

envBase.Install("test", execute)
Default(envBase.Install(prefix+"/bin/", execute))

