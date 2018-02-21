

cd $EFFORT_ENGINE/resource/shader

echo "" > $EFFORT_ENGINE/logs/shader_build.log

SHADER_TYPE="frag vert tesc tese geom comp"

#prepearing group folder | cleaning and creating folder group
rm -rf $EFFORT_ENGINE/resource/shader/ready/$1
mkdir $EFFORT_ENGINE/resource/shader/ready/$1

#compiling shaders from $1 group
for type in $SHADER_TYPE; do

    CURR_FILE=$EFFORT_ENGINE/resource/shader/scr/$1/shader.$type
    if [ -e $CURR_FILE ]; then
        make -f MakeFile.shader FILE_TO_COMPILE=$CURR_FILE FILE_OUTPUT=$EFFORT_ENGINE/resource/shader/ready/$1/$type.spv >> $EFFORT_ENGINE/logs/shader_build.log || exit 1
        echo "-ok $1/$type shader"
    else
        echo "-no $1/$type shader"
    fi

done

exit 0