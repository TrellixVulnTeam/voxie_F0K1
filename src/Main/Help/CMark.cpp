/*
 * Copyright (c) 2014-2022 The Voxie Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "CMark.hpp"

#include <VoxieClient/Exception.hpp>

#include <QtCore/QDebug>

using namespace vx::cmark;

String::String(char* data) : data_(data) {}
// TODO: Does this work properly under windows? (i.e. will this use the same c
// library as cmark?)
String::~String() {
  if (data_) free(data_);
}

QString String::createQString() { return QString::fromUtf8(data()); }

Node::Node(cmark_node* data, bool owned) : data_(data), owned_(owned) {}

Node::~Node() {
  if (owned_ && data_) cmark_node_free(data_);
}

QSharedPointer<Node> Node::newOwned(cmark_node* data) {
  if (!data) {
    // qWarning() << "vx::cmark::Node::newOwned called with nullptr data";
    // return QSharedPointer<Node>();
    throw vx::Exception(
        "de.uni_stuttgart.Voxie.InternalError",
        "Attempting to call vx::cmark::Node::newOwned with nullptr");
  }
  return QSharedPointer<Node>(new Node(data, true));
}

QSharedPointer<Node> Node::newUnowned(cmark_node* data) {
  if (!data) {
    // qWarning() << "vx::cmark::Node::newUnowned called with nullptr data";
    // return QSharedPointer<Node>();
    throw vx::Exception(
        "de.uni_stuttgart.Voxie.InternalError",
        "Attempting to call vx::cmark::Node::newUnowned with nullptr");
  }
  return QSharedPointer<Node>(new Node(data, false));
}

QSharedPointer<Node> Node::newUnownedAllowNull(cmark_node* data) {
  if (!data) return QSharedPointer<Node>();
  return QSharedPointer<Node>(new Node(data, false));
}

QSharedPointer<Node> Node::newNode(cmark_node_type type) {
  return newOwned(cmark_node_new(type));
}

QSharedPointer<Node> Node::parseDocument(const QString& buffer, int options) {
  auto bufferUtf8 = buffer.toUtf8();
  return vx::cmark::Node::newOwned(
      cmark_parse_document(bufferUtf8.data(), bufferUtf8.size(), options));
}

cmark_node* Node::steal() {
  // TODO: Use a lock here?
  if (!owned_) {
    throw vx::Exception(
        "de.uni_stuttgart.Voxie.InternalError",
        "Attempting to call Node::steal() for a non-owning node");
  }
  owned_ = false;
  return data_;
}

cmark_node_type Node::type() const { return cmark_node_get_type(data()); }

QString Node::getLiteral() const {
  return QString::fromUtf8(cmark_node_get_literal(data()));
}
void Node::setLiteral(const QString& str) const {
  if (!cmark_node_set_literal(data(), str.toUtf8().data()))
    qWarning() << "cmark_node_set_literal() failed";
}

QString Node::getOnEnter() const {
  return QString::fromUtf8(cmark_node_get_on_enter(data()));
}
void Node::setOnEnter(const QString& str) const {
  if (!cmark_node_set_on_enter(data(), str.toUtf8().data()))
    qWarning() << "cmark_node_set_on_enter() failed";
}
QString Node::getOnExit() const {
  return QString::fromUtf8(cmark_node_get_on_exit(data()));
}
void Node::setOnExit(const QString& str) const {
  if (!cmark_node_set_on_exit(data(), str.toUtf8().data()))
    qWarning() << "cmark_node_set_on_exit() failed";
}

QString Node::getUrl() const {
  return QString::fromUtf8(cmark_node_get_url(data()));
}
void Node::setUrl(const QString& str) const {
  if (!cmark_node_set_url(data(), str.toUtf8().data()))
    qWarning() << "cmark_node_set_url() failed";
}

QString Node::getTitle() const {
  return QString::fromUtf8(cmark_node_get_title(data()));
}
void Node::setTitle(const QString& str) const {
  if (!cmark_node_set_title(data(), str.toUtf8().data()))
    qWarning() << "cmark_node_set_title() failed";
}

QString Node::getFenceInfo() const {
  return QString::fromUtf8(cmark_node_get_fence_info(data()));
}
void Node::setFenceInfo(const QString& str) const {
  if (!cmark_node_set_fence_info(data(), str.toUtf8().data()))
    qWarning() << "cmark_node_set_fence_info() failed";
}

int Node::getHeadingLevel() const {
  return cmark_node_get_heading_level(data());
}
void Node::setHeadingLevel(int level) const {
  if (!cmark_node_set_heading_level(data(), level)) {
    qWarning() << "cmark_node_set_heading_level() failed";
  }
}

void* Node::getUserData() const { return cmark_node_get_user_data(data()); }
void Node::setUserData(void* data) const {
  if (!cmark_node_set_user_data(this->data(), data)) {
    qWarning() << "cmark_node_set_user_data() failed";
  }
}

cmark_list_type Node::getListType() const {
  return cmark_node_get_list_type(data());
}
void Node::setListType(cmark_list_type type) const {
  if (!cmark_node_set_list_type(data(), type)) {
    qWarning() << "cmark_node_set_list_type() failed";
  }
}

cmark_delim_type Node::getListDelim() const {
  return cmark_node_get_list_delim(data());
}
void Node::setListDelim(cmark_delim_type delim) const {
  if (!cmark_node_set_list_delim(data(), delim)) {
    qWarning() << "cmark_node_set_list_delim() failed";
  }
}

int Node::getListStart() const { return cmark_node_get_list_start(data()); }
void Node::setListStart(int start) const {
  if (!cmark_node_set_list_start(data(), start)) {
    qWarning() << "cmark_node_set_list_start() failed";
  }
}

bool Node::getListTight() const { return cmark_node_get_list_tight(data()); }
void Node::setListTight(bool tight) const {
  if (!cmark_node_set_list_tight(data(), tight ? 1 : 0)) {
    qWarning() << "cmark_node_set_list_tight() failed";
  }
}

QSharedPointer<Node> Node::previous() const {
  return newUnownedAllowNull(cmark_node_previous(data()));
}
QSharedPointer<Node> Node::next() const {
  return newUnownedAllowNull(cmark_node_next(data()));
}

void Node::unlink() {
  if (!cmark_node_parent(data()))
    throw vx::Exception("de.uni_stuttgart.Voxie.InternalError",
                        "Attempting to unlink node without parent");
  if (owned_)
    throw vx::Exception("de.uni_stuttgart.Voxie.InternalError",
                        "Attempting to unlink node owned node");
  cmark_node_unlink(data());
  owned_ = true;
}

QSharedPointer<Node> Node::firstChild() const {
  return newUnownedAllowNull(cmark_node_first_child(data()));
}

void Node::prependChild(const QSharedPointer<Node>& child) const {
  if (!cmark_node_prepend_child(data(), child->steal())) {
    qWarning() << "cmark_node_prepend_child() failed";
  }
}
void Node::appendChild(const QSharedPointer<Node>& child) const {
  if (!cmark_node_append_child(data(), child->steal())) {
    qWarning() << "cmark_node_append_child() failed";
  }
}

void Node::insertBefore(const QSharedPointer<Node>& silbing) const {
  if (!cmark_node_insert_before(data(), silbing->steal())) {
    qWarning() << "cmark_node_insert_before() failed";
  }
}
void Node::insertAfter(const QSharedPointer<Node>& silbing) const {
  if (!cmark_node_insert_after(data(), silbing->steal())) {
    qWarning() << "cmark_node_insert_after() failed";
  }
}

void Node::appendAllChildrenOf(const QSharedPointer<Node>& oldParent) {
  while (auto child = oldParent->firstChild()) {
    child->unlink();
    appendChild(child);
  }
}

QString Node::renderHtml(int options) const {
  vx::cmark::String html(cmark_render_html(data(), options));
  return html.createQString();
}
QString Node::renderXml(int options) const {
  vx::cmark::String xml(cmark_render_xml(data(), options));
  return xml.createQString();
}

QList<QSharedPointer<Node>> Node::children() const {
  QList<QSharedPointer<Node>> ret;

  for (cmark_node* child = cmark_node_first_child(this->data()); child;
       child = cmark_node_next(child))
    ret << Node::newUnowned(child);

  return ret;
}

QSharedPointer<Node> Node::cloneDeep() const {
  auto type = this->type();
  auto clone = newNode(type);

  // Should the user data be copied?
  clone->setUserData(this->getUserData());

  switch (type) {
    case CMARK_NODE_HTML_BLOCK:
    case CMARK_NODE_TEXT:
    case CMARK_NODE_HTML_INLINE:
    case CMARK_NODE_CODE:
    case CMARK_NODE_CODE_BLOCK:
      clone->setLiteral(this->getLiteral());
    default:
      break;
  }

  if (type == CMARK_NODE_HEADING)
    clone->setHeadingLevel(this->getHeadingLevel());

  if (type == CMARK_NODE_LIST) {
    clone->setListType(this->getListType());
    if (this->getListDelim()) clone->setListDelim(this->getListDelim());
    clone->setListStart(this->getListStart());
    clone->setListTight(this->getListTight());
  }

  if (type == CMARK_NODE_CODE_BLOCK) clone->setFenceInfo(this->getFenceInfo());

  if ((type == CMARK_NODE_LINK || type == CMARK_NODE_IMAGE)) {
    clone->setUrl(this->getUrl());
    clone->setTitle(this->getTitle());
  }

  if (type == CMARK_NODE_CUSTOM_BLOCK || type == CMARK_NODE_CUSTOM_INLINE) {
    clone->setOnEnter(this->getOnEnter());
    clone->setOnExit(this->getOnExit());
  }

  // TODO: start / end line / column?

  // Clone children
  for (const auto& child : this->children())
    clone->appendChild(child->cloneDeep());

  if (0) {
    auto thisXml = this->renderXml();
    auto cloneXml = clone->renderXml();
    if (thisXml != cloneXml)
      qWarning() << "Cloning changed node:" << thisXml << cloneXml;
  }

  return clone;
}
