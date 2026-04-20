// swift-tools-version:5.5

import PackageDescription

let package = Package(
    name: "cmark",
    products: [
        .library(name: "cmark", targets: ["cmark"]),
    ],
    targets: [
        .target(
            name: "cmark",
            path: "Sources/cmark",
            publicHeadersPath: "include",
            cSettings: [
                .headerSearchPath("."),
                .headerSearchPath("include"),
                .define("CMARK_GFM_STATIC_DEFINE"),
            ]
        )
    ]
)
