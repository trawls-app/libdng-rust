extern crate cmake;
use cmake::Config;
use std::env;


fn main() {
    let dst = Config::new("src/cpp").build();

    println!("cargo:rustc-link-search=native={}", dst.display());
    println!("cargo:rustc-link-lib=static=dng");
    println!("cargo:rustc-link-lib=static=dng_sdk");
    println!("cargo:rustc-link-lib=static=xmp-sdk");
    println!("cargo:rustc-link-lib=dylib=jpeg");
    println!("cargo:rustc-link-lib=dylib=expat");
    println!("cargo:rustc-link-lib=dylib=z");

    let target  = env::var("TARGET").unwrap();
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
