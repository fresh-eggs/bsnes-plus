#ifndef NALL_STRING_XML_HPP
#define NALL_STRING_XML_HPP

//XML subset parser
//version 0.05

namespace nall {

struct xml_attribute {
  string name;
  string content;
  virtual string parse() const;
};

struct xml_element : xml_attribute {
  string parse() const;
  linear_vector<xml_attribute> attribute;
  linear_vector<xml_element> element;

protected:
  void parse_doctype(const char *&data);
  void parse_head(string data);
  bool parse_body(const char *&data);
  friend xml_element xml_parse(const char *data);
};

inline string xml_attribute::parse() const {
  string data;
  unsigned offset = 0;

  const char *source = content;
  while(*source) {
    if(*source == '&') {
      if(strbegin(source, "&lt;"))   { data[offset++] = '<';  source += 4; continue; }
      if(strbegin(source, "&gt;"))   { data[offset++] = '>';  source += 4; continue; }
      if(strbegin(source, "&amp;"))  { data[offset++] = '&';  source += 5; continue; }
      if(strbegin(source, "&apos;")) { data[offset++] = '\''; source += 6; continue; }
      if(strbegin(source, "&quot;")) { data[offset++] = '"';  source += 6; continue; }
    }

    //reject illegal characters
    if(*source == '&') return "";
    if(*source == '<') return "";
    if(*source == '>') return "";

    data[offset++] = *source++;
  }

  data[offset] = 0;
  return data;
}

inline string xml_element::parse() const {
  string data;
  unsigned offset = 0;

  const char *source = content;
  while(*source) {
    if(*source == '&') {
      if(strbegin(source, "&lt;"))   { data[offset++] = '<';  source += 4; continue; }
      if(strbegin(source, "&gt;"))   { data[offset++] = '>';  source += 4; continue; }
      if(strbegin(source, "&amp;"))  { data[offset++] = '&';  source += 5; continue; }
      if(strbegin(source, "&apos;")) { data[offset++] = '\''; source += 6; continue; }
      if(strbegin(source, "&quot;")) { data[offset++] = '"';  source += 6; continue; }
    }

    if(strbegin(source, "<!--")) {
      if(auto pos = strpos(source, "-->")) {
        source += pos() + 3;
        continue;
      } else {
        return "";
      }
    }

    if(strbegin(source, "<![CDATA[")) {
      if(auto pos = strpos(source, "]]>")) {
        if(pos() - 9 > 0) {
          string cdata = substr(source, 9, pos() - 9);
          data << cdata;
          offset += strlen(cdata);
        }
        source += 9 + offset + 3;
        continue;
      } else {
        return "";
      }
    }

    //reject illegal characters
    if(*source == '&') return "";
    if(*source == '<') return "";
    if(*source == '>') return "";

    data[offset++] = *source++;
  }

  data[offset] = 0;
  return data;
}

inline void xml_element::parse_doctype(const char *&data) {
  name = "!DOCTYPE";
  const char *content_begin = data;

  signed counter = 0;
  while(*data) {
    char value = *data++;
    if(value == '<') counter++;
    if(value == '>') counter--;
    if(counter < 0) {
      content = substr(content_begin, 0, data - content_begin - 1);
      return;
    }
  }
  throw "...";
}

inline void xml_element::parse_head(string data) {
  data.qreplace("\t", " ");
  data.qreplace("\r", " ");
  data.qreplace("\n", " ");
  while(qstrpos(data, "  ")) data.qreplace("  ", " ");
  data.qreplace(" =", "=");
  data.qreplace("= ", "=");
  data.rtrim();

  lstring part;
  part.qsplit(" ", data);

  name = part[0];
  if(name == "") throw "...";

  for(unsigned i = 1; i < part.size(); i++) {
    lstring side;
    side.qsplit("=", part[i]);
    if(side.size() != 2) throw "...";

    xml_attribute attr;
    attr.name = side[0];
    attr.content = side[1];
    if(strbegin(attr.content, "\"") && strend(attr.content, "\"")) attr.content.trim<1>("\"");
    else if(strbegin(attr.content, "'") && strend(attr.content, "'")) attr.content.trim<1>("'");
    else throw "...";
    attribute.append(attr);
  }
}

inline bool xml_element::parse_body(const char *&data) {
  fprintf(stderr, "[*][xml.hpp][parse_body] start, data: %s\n", data);
  while(true) {
    if(!*data) return false;
    if(*data++ != '<') continue;
    if(*data == '/') return false;

    if(strbegin(data, "!DOCTYPE") == true) {
      parse_doctype(data);
      return true;
    }

    if(strbegin(data, "!--")) {
      if(auto offset = strpos(data, "-->")) {
        data += offset() + 3;
        continue;
      } else {
        throw "...";
      }
    }

    if(strbegin(data, "![CDATA[")) {
      if(auto offset = strpos(data, "]]>")) {
        data += offset() + 3;
        continue;
      } else {
        throw "...";
      }
    }

    auto offset = strpos(data, ">");
    if(!offset) throw "...";

    string tag = substr(data, 0, offset());
    data += offset() + 1;
    const char *content_begin = data;

    bool self_terminating = false;

    if(strend(tag, "?") == true) {
      self_terminating = true;
      tag.rtrim<1>("?");
    } else if(strend(tag, "/") == true) {
      self_terminating = true;
      tag.rtrim<1>("/");
    }

    parse_head(tag);
    if(self_terminating) return true;
    
    fprintf(stderr, "[*][xml.hpp:199][parse_body] Parsing tag: %s\n", (const char*)tag);
    while(*data) {
      unsigned index = element.size();
      xml_element node;
      if(node.parse_body(data) == false) {
        fprintf(stderr, "[*][xml.hpp:204][parse_body] Recursive pasrse_body was false\n: data: %s\n node: %s\n",data, (const char*)node.content);
        if(*data == '/') {
          signed length = data - content_begin - 1;
          if(length > 0) content = substr(content_begin, 0, length);

          data++;
          auto offset = strpos(data, ">");
          if(!offset) throw "...";

          tag = substr(data, 0, offset());
          data += offset() + 1;

          tag.replace("\t", " ");
          tag.replace("\r", " ");
          tag.replace("\n", " ");
          while(strpos(tag, "  ")) tag.replace("  ", " ");
          tag.rtrim();

          if(name != tag) throw "...";
          return true;
        }
      } else {
        fprintf(stderr, "[*][xml.hpp:225][parse_body] Appending node:\n %s\n", (const char*)node.content);
        element.append(node);
      }
    }
  }
}

//ensure there is only one root element
inline bool xml_validate(xml_element &document) {
  unsigned root_counter = 0;

  for(unsigned i = 0; i < document.element.size(); i++) {
    string &name = document.element[i].name;
    if(strbegin(name, "?")) continue;
    if(strbegin(name, "!")) continue;
    if(++root_counter > 1) return false;
  }

  return true;
}

// parses the XML document you pass and appends nodes to the element array of self.
inline xml_element xml_parse(const char *data) {
  fprintf(stderr, "[*][xml.hpp][xml_parse] start, data: %s\n", data);
  xml_element self;
  
  try {
    while(*data) {
      xml_element node;
      if(node.parse_body(data) == false) {
        fprintf(stderr, "[*][xml.hpp][xml_parse] parse_body was false\n");
        break;
      } else {
        fprintf(stderr, "[*][xml.hpp][xml_parse] parse_body was true\n");
        self.element.append(node);
      }
    }

    if(xml_validate(self) == false) throw "...";
    return self;
  } catch(const char*) {
    xml_element empty;
    return empty;
  }
}

}

#endif
