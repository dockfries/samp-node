#pragma once

// clang-format off
const std::string bootstrap = R"(
const { pathToFileURL } = require("url");
const { setDefaultResultOrder } = require("dns");

(async () => {
  setDefaultResultOrder("ipv4first");
  try {
    const entryPath = __internal_resource.entryFile;
    await import(pathToFileURL(entryPath).toString());
  } catch (e) {
    console.error(e);
  }
  __internal_esmLoaded();
})();
)";