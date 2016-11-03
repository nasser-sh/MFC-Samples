/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */

(function () {
    // By default, the WebBrowser control supports IE6.
    "use strict";

    function main() {
        var body = document.getElementsByTagName("body")[0];
        body.innerHTML += "<p>This was added via JavaScript</p>";
    }

    window.attachEvent("onload", main);
}());