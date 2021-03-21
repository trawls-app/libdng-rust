extern crate cmake;
use cmake::Config;
use std::env;


fn main() {
    let target  = env::var("TARGET").unwrap();

    let dst = if target.contains("apple") || target.contains("linux") {
        Config::new("src/cpp").build()
    } else {
        Config::new("src/cpp").static_crt(false).define("VCPKG_TARGET_TRIPLET", "x64-windows-static").build()
    };

    let path = dst.display();

    println!("cargo:rustc-link-search=native={}/lib", path);
    println!("cargo:rustc-link-lib=static=dngbindings");
    println!("cargo:rustc-link-lib=static=dng-sdk");
    println!("cargo:rustc-link-lib=static=xmp-sdk");

    if target.contains("apple")
    {
        println!("cargo:rustc-link-lib=dylib=c++");
        println!("cargo:rustc-link-search=native={}/build/vcpkg_installed/x64-osx/lib", path);
        println!("cargo:rustc-link-lib=static=expat");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=z");
    }
    else if target.contains("linux")
    {
        println!("cargo:rustc-link-lib=dylib=stdc++");
        println!("cargo:rustc-link-search=native={}/build/vcpkg_installed/x64-linux/lib", path);
        println!("cargo:rustc-link-lib=static=expat");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=z");
    }
    else
    {
        println!("cargo:rustc-link-search=native={}/build/vcpkg_installed/x64-windows-static/lib", path);
        println!("cargo:rustc-link-lib=static=libexpatMD");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=zlib");
    }
}
