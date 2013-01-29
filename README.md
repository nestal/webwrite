\mainpage

WebWrite is a wiki-like content management system written in HTML5/Javascript/C++.
WebWrite uses [ckeditor](http://ckeditor.com/) for in-place HTML editing. Unlike
other wiki implementation, the pages in WebWrite can be edited with a
what-you-see-is-what-you-get HTML editor. There is no need to learn to type special
wiki-text language like [markdown](http://daringfireball.net/projects/markdown/).

Design Principle
================

WebWrite is designed to be intuitive and simple. Most operations in WebWrite are
designed to be similar to file systems. Resources in WebWrite is organized in a
directory tree like a file system, and they are stored in the server in the same
structure. It is easy for the users and administrators to understand how and
where their data are stored.

Although WebWrite is a dynamic web site, most pages are served as-is without any
transformation in the server side. This ensure the server has minimum loading.
The pages in stored in the server in HTML, which can be directly rendered in the
web browser without any further processing.
  
WebWrite uses [X-Accel-Redirect](http://wiki.nginx.org/XSendfile) to serve static
files. It is currently supported by nginx only (AFAIK). For other web servers,
WebWrite can serve the files by itself, but it would not have the same performance
as using internal re-directs.

Dependencies
============

Required:

- [Boost library](http://boost.org)
- [JSON-C](https://github.com/json-c/json-c)
- [FastCGI development kit](http://www.fastcgi.com/devkit/doc/fcgi-devel-kit.htm)
- [libxml2](http://www.xmlsoft.org/index.html) (optional, but recommended for security)
- [ckeditor](http://ckeditor.com/)


REST API
========

Assumed that webwrite is installed at http://example.com/webwrite/

In general, the URL of webwrite can be decomposed into these parts:

	
    http://example.com/webwrite/some/long/path/to/the/page?load
                                ^-----resource path------^ ^--^-command

Resource
========

A resource is a basic unit of data that can be accessed from in WebWrite. 

The resource path in the URL locates the resource in the server.
