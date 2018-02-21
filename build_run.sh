
function exit_make_error(){
    echo
    echo "> make error build"
    echo "> stop."
    exit 1
}

cd build

echo
echo "> cmake prosessing..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

echo
echo "> make prosessing..."

make || exit_make_error

mv effort_engine ../effort_engine
cd ..

#compile shaders
echo
echo "> compiling shaders..."
resource/shader/compile_shader.sh default
if [[ 0 != $? ]]; then
    echo "> compiling shaders error."
    echo "> stop."
    exit 1
fi

echo
echo "> launching programm"
sleep 1
clear
./effort_engine

echo
echo "> stop."
