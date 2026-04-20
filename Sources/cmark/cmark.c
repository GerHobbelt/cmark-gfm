#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "registry.h"
#include "node.h"
#include "houdini.h"
#include "cmark-gfm.h"
#include "cmark-gfm-core-extensions.h"
#include "buffer.h"
#include "parser.h"

cmark_node_type CMARK_NODE_LAST_BLOCK = CMARK_NODE_FOOTNOTE_DEFINITION;
cmark_node_type CMARK_NODE_LAST_INLINE = CMARK_NODE_FOOTNOTE_REFERENCE;

int cmark_version(void) { return CMARK_GFM_VERSION; }

const char *cmark_version_string(void) { return CMARK_GFM_VERSION_STRING; }

static void *xcalloc(size_t nmem, size_t size) {
  void *ptr = calloc(nmem, size);
  if (!ptr) {
    fprintf(stderr, "[cmark] calloc returned null pointer, aborting\n");
    abort();
  }
  return ptr;
}

static void *xrealloc(void *ptr, size_t size) {
  void *new_ptr = realloc(ptr, size);
  if (!new_ptr) {
    fprintf(stderr, "[cmark] realloc returned null pointer, aborting\n");
    abort();
  }
  return new_ptr;
}

static void xfree(void *ptr) {
  free(ptr);
}

cmark_mem CMARK_DEFAULT_MEM_ALLOCATOR = {xcalloc, xrealloc, xfree};

cmark_mem *cmark_get_default_mem_allocator(void) {
  return &CMARK_DEFAULT_MEM_ALLOCATOR;
}

char *cmark_markdown_to_html(const char *text, size_t len, int options) {
  cmark_node *doc;
  char *result;

  doc = cmark_parse_document(text, len, options);

  result = cmark_render_html(doc, options, NULL);
  cmark_node_free(doc);

  return result;
}

char *cmark_gfm_markdown_to_html(const char *text, size_t len, int options) {
  cmark_parser *parser;
  cmark_node *doc;
  char *result;
  static const char *const extension_names[] = {
    "autolink", "strikethrough", "table", "tagfilter", "tasklist",
  };

  cmark_gfm_core_extensions_ensure_registered();

  parser = cmark_parser_new(options);
  for (size_t i = 0; i < sizeof(extension_names) / sizeof(extension_names[0]); i++) {
    cmark_syntax_extension *ext = cmark_find_syntax_extension(extension_names[i]);
    if (ext != NULL) {
      cmark_parser_attach_syntax_extension(parser, ext);
    }
  }

  cmark_parser_feed(parser, text, len);
  doc = cmark_parser_finish(parser);

  result = cmark_render_html(doc, options, parser->syntax_extensions);
  cmark_node_free(doc);
  cmark_parser_free(parser);

  return result;
}
