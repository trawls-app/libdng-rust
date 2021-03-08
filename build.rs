extern crate cmake;
use cmake::Config;
use std::env;


fn main() {
    let target  = env::var("TARGET").unwrap();

    let dst = if target.contains("apple") || target.contains("linux") {
        Config::new("src/cpp").very_verbose(true).define("CMAKE_TOOLCHAIN_FILE", "vcpkg/scripts/buildsystems/vcpkg.cmake").build()
    } else {
        Config::new("src/cpp").define("CMAKE_CXX_COMPILER", "g++").define("CMAKE_CC_COMPILER", "gcc").host("").build()
    };

    println!("cargo:rustc-link-search=native={}", dst.display());
    println!("cargo:rustc-link-lib=static=dng");
    println!("cargo:rustc-link-lib=static=dng_sdk");
    println!("cargo:rustc-link-lib=static=xmp-sdk");
    println!("cargo:rustc-link-lib=static=libjpeg");
    println!("cargo:rustc-link-lib=dylib=expat");
    //println!("cargo:rustc-link-lib=static=zlibstatic");
    vcpkg::find_package("zlib").unwrap();


    if target.contains("apple")
    {
        println!("cargo:rustc-link-lib=dylib=c++");
    }
    else if target.contains("linux")
    {
        println!("cargo:rustc-link-lib=dylib=stdc++");
    }
    else
    {
        println!("cargo:rustc-link-lib=dylib=c++14");
    }
}
