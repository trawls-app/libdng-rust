extern crate cmake;
use cmake::Config;
use std::env;


fn main() {
    let target  = env::var("TARGET").unwrap();

    let dst = if target.contains("apple") || target.contains("linux") {
        Config::new("src/cpp").build()
    } else {
        Config::new("src/cpp").generator("MinGW Makefiles").build()
    };

    println!("cargo:rustc-link-search=native={}", dst.display());
    println!("cargo:rustc-link-lib=static=dng");
    println!("cargo:rustc-link-lib=static=dng_sdk");
    println!("cargo:rustc-link-lib=static=xmp-sdk");
    println!("cargo:rustc-link-lib=static=libjpeg");
    println!("cargo:rustc-link-lib=dylib=expat");
    println!("cargo:rustc-link-lib=static=z");


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
