extern crate cmake;
use cmake::Config;
use std::env;


fn main() {
    let target  = env::var("TARGET").unwrap();

    let dst = Config::new("src/cpp").build();

    println!("cargo:rustc-link-search=native={}", dst.display());
    println!("cargo:rustc-link-lib=static=dngbindings");


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
        //println!("cargo:rustc-link-lib=dylib=stdc++");
    }
}
