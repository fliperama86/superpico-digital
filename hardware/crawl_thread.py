#!/usr/bin/env python3
"""
Convert saved phpBB forum HTML pages into a single Markdown file.
Usage: python3 crawl_thread.py /path/to/forum/dir [-o output.md]
"""

import argparse
import glob
import html
import os
import re
from html.parser import HTMLParser


# ---------------------------------------------------------------------------
# HTML → Markdown converter
# ---------------------------------------------------------------------------
class HTML2Text(HTMLParser):
    def __init__(self):
        super().__init__()
        self.result = []
        self._in_pre = False
        self._in_a = False
        self._href = ""
        self._link_text = []
        self._list_depth = 0
        self._bold = False
        self._italic = False

    def handle_starttag(self, tag, attrs):
        attrs_d = dict(attrs)
        cls = attrs_d.get("class", "")
        if tag == "br":
            self.result.append("\n")
        elif tag in ("p", "div"):
            self.result.append("\n\n")
        elif tag == "blockquote":
            self.result.append("\n\n> ")
        elif tag == "pre" or (tag == "code" and cls):
            self._in_pre = True
            self.result.append("\n```\n")
        elif tag == "a":
            self._in_a = True
            self._href = attrs_d.get("href", "")
            self._link_text = []
        elif tag in ("b", "strong"):
            self.result.append("**")
            self._bold = True
        elif tag in ("i", "em"):
            # Skip font-awesome icons
            if "fa-" in cls or "icon" in cls:
                return
            self.result.append("*")
            self._italic = True
        elif tag == "li":
            self.result.append("\n" + "  " * self._list_depth + "- ")
        elif tag in ("ul", "ol"):
            self._list_depth += 1
        elif tag == "img":
            src = attrs_d.get("src", "")
            alt = attrs_d.get("alt", "image")
            if src and "smilies" not in src and "icon" not in cls:
                self.result.append(f"![{alt}]({src})")

    def handle_endtag(self, tag):
        if tag == "blockquote":
            self.result.append("\n\n")
        elif tag == "pre" or (tag == "code" and self._in_pre):
            self._in_pre = False
            self.result.append("\n```\n")
        elif tag == "a" and self._in_a:
            self._in_a = False
            text = "".join(self._link_text).strip()
            if self._href and text and self._href != text:
                self.result.append(f"[{text}]({self._href})")
            elif text:
                self.result.append(text)
        elif tag in ("b", "strong") and self._bold:
            self.result.append("**")
            self._bold = False
        elif tag in ("i", "em") and self._italic:
            self.result.append("*")
            self._italic = False
        elif tag in ("ul", "ol"):
            self._list_depth = max(0, self._list_depth - 1)

    def handle_data(self, data):
        if self._in_a:
            self._link_text.append(data)
        else:
            self.result.append(data)

    def handle_entityref(self, name):
        self.handle_data(html.unescape(f"&{name};"))

    def handle_charref(self, name):
        self.handle_data(html.unescape(f"&#{name};"))

    def get_text(self):
        text = "".join(self.result)
        text = re.sub(r"\n{3,}", "\n\n", text)
        return text.strip()


def html_to_md(html_str: str) -> str:
    p = HTML2Text()
    p.feed(html_str)
    return p.get_text()


# ---------------------------------------------------------------------------
# Post extractor from saved HTML
# ---------------------------------------------------------------------------
class PostExtractor(HTMLParser):
    """Extract posts from a saved phpBB HTML page."""

    def __init__(self):
        super().__init__()
        self.posts: list[dict] = []
        self.title = ""

        # State tracking
        self._in_postbody = False
        self._postbody_depth = 0

        self._in_author_p = False
        self._in_username = False
        self._in_content = False
        self._content_depth = 0
        self._in_title = False

        self._current_author = ""
        self._current_date = ""
        self._content_parts: list[str] = []
        self._author_text_parts: list[str] = []

    def _reset_post(self):
        self._current_author = ""
        self._current_date = ""
        self._content_parts = []
        self._author_text_parts = []

    def handle_starttag(self, tag, attrs):
        attrs_d = dict(attrs)
        cls = attrs_d.get("class", "")

        # Topic title
        if tag == "h2" and "topic-title" in cls:
            self._in_title = True

        # Post body container
        if tag == "div" and "postbody" in cls:
            self._in_postbody = True
            self._postbody_depth = 1
            self._reset_post()
            return

        if not self._in_postbody:
            return

        if tag == "div":
            self._postbody_depth += 1

        # Author paragraph
        if tag == "p" and "author" in cls:
            self._in_author_p = True
            self._author_text_parts = []

        # Username link inside author paragraph
        if self._in_author_p and tag == "a" and ("username" in cls or "username-coloured" in cls):
            self._in_username = True

        # Content div
        if tag == "div" and "content" == cls.strip():
            self._in_content = True
            self._content_depth = 1
            self._content_parts = []
            return

        if self._in_content:
            if tag == "div":
                self._content_depth += 1
            # Reconstruct inner HTML
            attr_str = " ".join(f'{k}="{v}"' for k, v in attrs
                                if not k.startswith("data-darkreader"))
            self._content_parts.append(f"<{tag} {attr_str}>" if attr_str else f"<{tag}>")

    def handle_endtag(self, tag):
        if self._in_title and tag == "h2":
            self._in_title = False

        if not self._in_postbody:
            return

        # Username
        if self._in_username and tag == "a":
            self._in_username = False

        # Author paragraph
        if self._in_author_p and tag == "p":
            self._in_author_p = False
            # Extract date from author text: "by Author » date"
            raw = "".join(self._author_text_parts)
            if "»" in raw:
                self._current_date = raw.split("»")[-1].strip()

        # Content div
        if self._in_content:
            if tag == "div":
                self._content_depth -= 1
                if self._content_depth <= 0:
                    self._in_content = False
                    return
            self._content_parts.append(f"</{tag}>")

        # Post body container end
        if tag == "div":
            self._postbody_depth -= 1
            if self._postbody_depth <= 0:
                self._in_postbody = False
                content = "".join(self._content_parts)
                if content.strip():
                    self.posts.append({
                        "author": self._current_author,
                        "date": self._current_date,
                        "content": content,
                    })

    def handle_data(self, data):
        if self._in_title:
            self.title += data

        if self._in_username:
            self._current_author = data.strip()

        if self._in_author_p:
            self._author_text_parts.append(data)

        if self._in_content:
            self._content_parts.append(html.escape(data))

    def handle_entityref(self, name):
        if self._in_content:
            self._content_parts.append(f"&{name};")
        if self._in_author_p:
            self._author_text_parts.append(html.unescape(f"&{name};"))

    def handle_charref(self, name):
        if self._in_content:
            self._content_parts.append(f"&#{name};")
        if self._in_author_p:
            self._author_text_parts.append(html.unescape(f"&#{name};"))


def extract_posts(html_content: str) -> tuple[list[dict], str]:
    p = PostExtractor()
    p.feed(html_content)
    return p.posts, p.title.strip()


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="Convert saved phpBB HTML pages to a single Markdown file")
    parser.add_argument("input_dir", help="Directory with saved HTML files")
    parser.add_argument("-o", "--output", default=None,
                        help="Output markdown file (default: <input_dir>/thread.md)")
    args = parser.parse_args()

    html_files = sorted(glob.glob(os.path.join(args.input_dir, "*.html")))
    if not html_files:
        print(f"No HTML files found in {args.input_dir}")
        return

    # Sort: page without number first, then by page number
    def page_sort_key(path):
        name = os.path.basename(path)
        m = re.search(r"Page\s+(\d+)", name)
        return int(m.group(1)) if m else 0

    html_files.sort(key=page_sort_key)

    print(f"Found {len(html_files)} HTML file(s)")

    all_posts = []
    title = ""

    for html_file in html_files:
        print(f"  Parsing: {os.path.basename(html_file)}")
        with open(html_file, "r", encoding="utf-8", errors="replace") as f:
            content = f.read()
        posts, page_title = extract_posts(content)
        if page_title and not title:
            title = page_title
        print(f"    → {len(posts)} posts")
        all_posts.extend(posts)

    if not title:
        title = "Forum Thread"

    # Build single markdown
    lines = [f"# {title}\n"]
    for i, post in enumerate(all_posts, 1):
        author = post["author"] or "Unknown"
        date = post["date"] or ""
        content_md = html_to_md(post["content"])
        lines.append(f"---\n\n## #{i} — {author}")
        if date:
            lines.append(f"*{date}*\n")
        lines.append(f"\n{content_md}\n")

    output_path = args.output or os.path.join(args.input_dir, "thread.md")
    with open(output_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

    print(f"\nDone! {len(all_posts)} posts saved to {output_path}")


if __name__ == "__main__":
    main()
