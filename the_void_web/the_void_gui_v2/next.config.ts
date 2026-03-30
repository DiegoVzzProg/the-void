import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  allowedDevOrigins: ['169.254.20.69'],
  reactCompiler: true,
  webpack(config) {
    config.module.rules.push({
      test: /\.svg$/i,
      issuer: /\.[jt]sx?$/,
      use: ['@svgr/webpack'],
    })

    return config
  },

  turbopack: {
    rules: {
      "*.svg": {
        loaders: ["@svgr/webpack"],
        as: "*.js"
      }
    }
  }
};

export default nextConfig;
