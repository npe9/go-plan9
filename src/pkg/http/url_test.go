// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package http

import (
	"fmt"
	"os"
	"reflect"
	"testing"
)

// TODO(rsc):
//	test URLUnescape
//	test URLEscape
//	test ParseURL

type URLTest struct {
	in        string
	out       *URL
	roundtrip string // expected result of reserializing the URL; empty means same as "in".
}

var urltests = []URLTest{
	// no path
	URLTest{
		"http://www.google.com",
		&URL{
			Raw:       "http://www.google.com",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
		},
		"",
	},
	// path
	URLTest{
		"http://www.google.com/",
		&URL{
			Raw:       "http://www.google.com/",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
			RawPath:   "/",
			Path:      "/",
		},
		"",
	},
	// path with hex escaping
	URLTest{
		"http://www.google.com/file%20one%26two",
		&URL{
			Raw:       "http://www.google.com/file%20one%26two",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
			RawPath:   "/file%20one%26two",
			Path:      "/file one&two",
		},
		"http://www.google.com/file%20one%26two",
	},
	// user
	URLTest{
		"ftp://webmaster@www.google.com/",
		&URL{
			Raw:       "ftp://webmaster@www.google.com/",
			Scheme:    "ftp",
			Authority: "webmaster@www.google.com",
			Userinfo:  "webmaster",
			Host:      "www.google.com",
			RawPath:   "/",
			Path:      "/",
		},
		"",
	},
	// escape sequence in username
	URLTest{
		"ftp://john%20doe@www.google.com/",
		&URL{
			Raw:       "ftp://john%20doe@www.google.com/",
			Scheme:    "ftp",
			Authority: "john doe@www.google.com",
			Userinfo:  "john doe",
			Host:      "www.google.com",
			RawPath:   "/",
			Path:      "/",
		},
		"ftp://john%20doe@www.google.com/",
	},
	// query
	URLTest{
		"http://www.google.com/?q=go+language",
		&URL{
			Raw:       "http://www.google.com/?q=go+language",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
			RawPath:   "/?q=go+language",
			Path:      "/",
			RawQuery:  "q=go+language",
		},
		"",
	},
	// query with hex escaping: NOT parsed
	URLTest{
		"http://www.google.com/?q=go%20language",
		&URL{
			Raw:       "http://www.google.com/?q=go%20language",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
			RawPath:   "/?q=go%20language",
			Path:      "/",
			RawQuery:  "q=go%20language",
		},
		"",
	},
	// %20 outside query
	URLTest{
		"http://www.google.com/a%20b?q=c+d",
		&URL{
			Raw:       "http://www.google.com/a%20b?q=c+d",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
			RawPath:   "/a%20b?q=c+d",
			Path:      "/a b",
			RawQuery:  "q=c+d",
		},
		"",
	},
	// path without /, so no query parsing
	URLTest{
		"http:www.google.com/?q=go+language",
		&URL{
			Raw:     "http:www.google.com/?q=go+language",
			Scheme:  "http",
			RawPath: "www.google.com/?q=go+language",
			Path:    "www.google.com/?q=go+language",
		},
		"http:www.google.com/%3fq%3dgo%2blanguage",
	},
	// non-authority
	URLTest{
		"mailto:/webmaster@golang.org",
		&URL{
			Raw:     "mailto:/webmaster@golang.org",
			Scheme:  "mailto",
			RawPath: "/webmaster@golang.org",
			Path:    "/webmaster@golang.org",
		},
		"",
	},
	// non-authority
	URLTest{
		"mailto:webmaster@golang.org",
		&URL{
			Raw:     "mailto:webmaster@golang.org",
			Scheme:  "mailto",
			RawPath: "webmaster@golang.org",
			Path:    "webmaster@golang.org",
		},
		"",
	},
	// unescaped :// in query should not create a scheme
	URLTest{
		"/foo?query=http://bad",
		&URL{
			Raw:      "/foo?query=http://bad",
			RawPath:  "/foo?query=http://bad",
			Path:     "/foo",
			RawQuery: "query=http://bad",
		},
		"",
	},
}

var urlnofragtests = []URLTest{
	URLTest{
		"http://www.google.com/?q=go+language#foo",
		&URL{
			Raw:       "http://www.google.com/?q=go+language#foo",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
			RawPath:   "/?q=go+language#foo",
			Path:      "/",
			RawQuery:  "q=go+language#foo",
		},
		"",
	},
}

var urlfragtests = []URLTest{
	URLTest{
		"http://www.google.com/?q=go+language#foo",
		&URL{
			Raw:       "http://www.google.com/?q=go+language#foo",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
			RawPath:   "/?q=go+language#foo",
			Path:      "/",
			RawQuery:  "q=go+language",
			Fragment:  "foo",
		},
		"",
	},
	URLTest{
		"http://www.google.com/?q=go+language#foo%26bar",
		&URL{
			Raw:       "http://www.google.com/?q=go+language#foo%26bar",
			Scheme:    "http",
			Authority: "www.google.com",
			Host:      "www.google.com",
			RawPath:   "/?q=go+language#foo%26bar",
			Path:      "/",
			RawQuery:  "q=go+language",
			Fragment:  "foo&bar",
		},
		"",
	},
}

// more useful string for debugging than fmt's struct printer
func ufmt(u *URL) string {
	return fmt.Sprintf("%q, %q, %q, %q, %q, %q, %q, %q, %q",
		u.Raw, u.Scheme, u.RawPath, u.Authority, u.Userinfo,
		u.Host, u.Path, u.RawQuery, u.Fragment)
}

func DoTest(t *testing.T, parse func(string) (*URL, os.Error), name string, tests []URLTest) {
	for _, tt := range tests {
		u, err := parse(tt.in)
		if err != nil {
			t.Errorf("%s(%q) returned error %s", name, tt.in, err)
			continue
		}
		if !reflect.DeepEqual(u, tt.out) {
			t.Errorf("%s(%q):\n\thave %v\n\twant %v\n",
				name, tt.in, ufmt(u), ufmt(tt.out))
		}
	}
}

func TestParseURL(t *testing.T) {
	DoTest(t, ParseURL, "ParseURL", urltests)
	DoTest(t, ParseURL, "ParseURL", urlnofragtests)
}

func TestParseURLReference(t *testing.T) {
	DoTest(t, ParseURLReference, "ParseURLReference", urltests)
	DoTest(t, ParseURLReference, "ParseURLReference", urlfragtests)
}

func DoTestString(t *testing.T, parse func(string) (*URL, os.Error), name string, tests []URLTest) {
	for _, tt := range tests {
		u, err := parse(tt.in)
		if err != nil {
			t.Errorf("%s(%q) returned error %s", name, tt.in, err)
			continue
		}
		s := u.String()
		expected := tt.in
		if len(tt.roundtrip) > 0 {
			expected = tt.roundtrip
		}
		if s != expected {
			t.Errorf("%s(%q).String() == %q (expected %q)", name, tt.in, s, expected)
		}
	}
}

func TestURLString(t *testing.T) {
	DoTestString(t, ParseURL, "ParseURL", urltests)
	DoTestString(t, ParseURL, "ParseURL", urlfragtests)
	DoTestString(t, ParseURL, "ParseURL", urlnofragtests)
	DoTestString(t, ParseURLReference, "ParseURLReference", urltests)
	DoTestString(t, ParseURLReference, "ParseURLReference", urlfragtests)
	DoTestString(t, ParseURLReference, "ParseURLReference", urlnofragtests)
}

type URLEscapeTest struct {
	in  string
	out string
	err os.Error
}

var unescapeTests = []URLEscapeTest{
	URLEscapeTest{
		"",
		"",
		nil,
	},
	URLEscapeTest{
		"abc",
		"abc",
		nil,
	},
	URLEscapeTest{
		"1%41",
		"1A",
		nil,
	},
	URLEscapeTest{
		"1%41%42%43",
		"1ABC",
		nil,
	},
	URLEscapeTest{
		"%4a",
		"J",
		nil,
	},
	URLEscapeTest{
		"%6F",
		"o",
		nil,
	},
	URLEscapeTest{
		"%", // not enough characters after %
		"",
		URLEscapeError("%"),
	},
	URLEscapeTest{
		"%a", // not enough characters after %
		"",
		URLEscapeError("%a"),
	},
	URLEscapeTest{
		"%1", // not enough characters after %
		"",
		URLEscapeError("%1"),
	},
	URLEscapeTest{
		"123%45%6", // not enough characters after %
		"",
		URLEscapeError("%6"),
	},
	URLEscapeTest{
		"%zzzzz", // invalid hex digits
		"",
		URLEscapeError("%zz"),
	},
}

func TestURLUnescape(t *testing.T) {
	for _, tt := range unescapeTests {
		actual, err := URLUnescape(tt.in)
		if actual != tt.out || (err != nil) != (tt.err != nil) {
			t.Errorf("URLUnescape(%q) = %q, %s; want %q, %s", tt.in, actual, err, tt.out, tt.err)
		}
	}
}

var escapeTests = []URLEscapeTest{
	URLEscapeTest{
		"",
		"",
		nil,
	},
	URLEscapeTest{
		"abc",
		"abc",
		nil,
	},
	URLEscapeTest{
		"one two",
		"one+two",
		nil,
	},
	URLEscapeTest{
		"10%",
		"10%25",
		nil,
	},
	URLEscapeTest{
		" ?&=#+%!<>#\"{}|\\^[]`☺\t",
		"+%3f%26%3d%23%2b%25!%3c%3e%23%22%7b%7d%7c%5c%5e%5b%5d%60%e2%98%ba%09",
		nil,
	},
}

func TestURLEscape(t *testing.T) {
	for _, tt := range escapeTests {
		actual := URLEscape(tt.in)
		if tt.out != actual {
			t.Errorf("URLEscape(%q) = %q, want %q", tt.in, actual, tt.out)
		}

		// for bonus points, verify that escape:unescape is an identity.
		roundtrip, err := URLUnescape(actual)
		if roundtrip != tt.in || err != nil {
			t.Errorf("URLUnescape(%q) = %q, %s; want %q, %s", actual, roundtrip, err, tt.in, "[no error]")
		}
	}
}

type CanonicalPathTest struct {
	in  string
	out string
}

var canonicalTests = []CanonicalPathTest{
	CanonicalPathTest{"", ""},
	CanonicalPathTest{"/", "/"},
	CanonicalPathTest{".", ""},
	CanonicalPathTest{"./", ""},
	CanonicalPathTest{"/a/", "/a/"},
	CanonicalPathTest{"a/", "a/"},
	CanonicalPathTest{"a/./", "a/"},
	CanonicalPathTest{"./a", "a"},
	CanonicalPathTest{"/a/../b", "/b"},
	CanonicalPathTest{"a/../b", "b"},
	CanonicalPathTest{"a/../../b", "../b"},
	CanonicalPathTest{"a/.", "a/"},
	CanonicalPathTest{"../.././a", "../../a"},
	CanonicalPathTest{"/../.././a", "/../../a"},
	CanonicalPathTest{"a/b/g/../..", "a/"},
	CanonicalPathTest{"a/b/..", "a/"},
	CanonicalPathTest{"a/b/.", "a/b/"},
	CanonicalPathTest{"a/b/../../../..", "../.."},
	CanonicalPathTest{"a./", "a./"},
	CanonicalPathTest{"/../a/b/../../../", "/../../"},
	CanonicalPathTest{"../a/b/../../../", "../../"},
}

func TestCanonicalPath(t *testing.T) {
	for _, tt := range canonicalTests {
		actual := CanonicalPath(tt.in)
		if tt.out != actual {
			t.Errorf("CanonicalPath(%q) = %q, want %q", tt.in, actual, tt.out)
		}
	}
}
