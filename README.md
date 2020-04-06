# EAD Solution Pack

## Introduction

This repository contains the EAD solution pack. This module provides
functionality for ingestion and management of EADs.

## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)
* [Tuque](https://github.com/islandora/tuque)
* [dgi_saxon_helper](https://github.com/discoverygarden/dgi_saxon_helper)

This module requires
[saxonb](http://manpages.ubuntu.com/manpages/trusty/man1/saxonb-xslt.1.html)
since it relies on XSLT 2.0. The `saxonb-xslt` binary is assumed to be at
`/usr/bin/saxonb-xslt` by default, but this is configurable in the
`dgi_saxon_helper` admin.

In addition, [Bootstrap](https://getbootstrap.com/docs/3.4/) and
[Popper.js](https://unpkg.com/popper.js@1.15.0/dist/umd/popper.min.js) should be 
downloaded and placed in sites/all/libraries:

* Bootstrap should be extracted into a libraries folder called 'bootstrap'
* `popper.min.js` should be placed in a libraries folder called 'popper.js'

In addition, if using bootstrap ensure [jQuery Update](
https://www.drupal.org/project/jquery_update) is installed and configured to use
jQuery 1.9.1 or later.

All finding aids should have their namespace
declared as `xmlns="urn:isbn:1-931666-22-9"`.

The module supports EAD 2002. It does not support EAD3.

## Installation

Install as usual, see
[this](https://drupal.org/documentation/install/modules-themes/modules-7) for
further information.

## Troubleshooting/Issues

Having problems or solved a problem? Contact [discoverygarden](
http://support.discoverygarden.ca).

## Maintainers/Sponsors

Current maintainers:

* [discoverygarden](http://www.discoverygarden.ca)

## Development

If you would like to contribute to this module, please check out our helpful
[Documentation for Developers](
https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers)
info, [Developers](http://islandora.ca/developers) section on Islandora.ca and
contact [discoverygarden](http://support.discoverygarden.ca).

## License

[GPLv2](http://www.gnu.org/licenses/gpl-2.0.txt)
