//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
//
// See the AUTHORS.md file for more details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "common.hh"
#include "edit.hh"
#include "edit_mode_classic.hh"

namespace drnsf {
namespace edit {
namespace mode_classic {

// (s-func) text_for_asset_type
// Takes a pointer to an NSF entry asset and returns an appropriate string to
// describe the dynamic type of the entry.
const char *text_for_asset_type(nsf::entry *entry)
{
    if (dynamic_cast<nsf::wgeo_v1 *>(entry)) {
        return "WGEO v1";
    } else if (dynamic_cast<nsf::wgeo_v2 *>(entry)) {
        return "WGEO v2";
    } else {
        return "Unrecognized";
    }
}

/*
 * edit::mode_classic::entry_ctlr
 *
 * FIXME explain
 */
class entry_ctlr : private util::nocopy {
private:
    // (var) m_index
    // The index of this controller within its parent. For example, if this is
    // `6', then this controller represents the 7th entry (`pagelets[6]') in the
    // page.
    int m_index;

    // (var) m_tree
    // A reference to the classic mode tree containing this controller's node.
    tree &m_tree;

    // (var) m_tracker
    // Tracks the asset name assigned to this controller.
    res::tracker<res::asset> m_tracker;

    // (var) m_node
    // The tree node associated with this controller.
    gui::treeview::node m_node;

    // (handler) h_eid_change
    // Watches the attached asset, if it is an entry, to update the treenode
    // text when the entry's eid changes.
    decltype(decltype(nsf::entry::p_eid)::on_change)::watch h_eid_change;

    // (handler) h_type_change
    // Watches the attached asset, if it is a raw_entry, to update the treenode
    // text when the entry's type number changes.
    decltype(decltype(nsf::raw_entry::p_type)::on_change)::watch h_type_change;

    // (func) update_node_text
    // Sets the text for the tree node based on the state of the asset.
    void update_node_text(res::asset *asset)
    {
        if (!asset) {
            m_node.set_text("(pagelet #$) Nothing"_fmt(m_index));
        } else if (auto raw_entry = dynamic_cast<nsf::raw_entry *>(asset)) {
            m_node.set_text("(pagelet #$) $ (Raw T$)"_fmt(
                m_index,
                raw_entry->get_eid(),
                raw_entry->get_type()
            ));
        } else if (auto entry = dynamic_cast<nsf::entry *>(asset)) {
            m_node.set_text("(pagelet #$) $ ($)"_fmt(
                m_index,
                entry->get_eid(),
                text_for_asset_type(entry)
            ));
        } else if (dynamic_cast<misc::raw_data *>(asset)) {
            m_node.set_text("(pagelet #$) Raw Data"_fmt(m_index));
        } else {
            m_node.set_text("(pagelet #$) Incompatible Asset"_fmt(m_index));
        }
    }

public:
    // (explicit ctor)
    // FIXME explain
    explicit entry_ctlr(tree &tree, gui::treeview::node &parent, int index) :
        m_index(index),
        m_tree(tree),
        m_node(parent)
    {
        update_node_text(nullptr);

        // Hook treenode select/deselect events to pass the selected asset on
        // to the owner of the tree.
        m_node.on_select <<= [this]{
            m_tree.on_select(m_tracker.get_name());
        };
        m_node.on_deselect <<= [this]{
            m_tree.on_select(nullptr);
        };

        h_eid_change <<= [this]{
            update_node_text(static_cast<nsf::entry *>(
                m_tracker.get_name().get()
            ));
        };

        h_type_change <<= [this]{
            update_node_text(static_cast<nsf::entry *>(
                m_tracker.get_name().get()
            ));
        };

        m_tracker.on_acquire <<= [this](res::asset *asset) {
            update_node_text(asset);

            if (auto raw_entry = dynamic_cast<nsf::raw_entry *>(asset)) {
                h_eid_change.bind(raw_entry->p_eid.on_change);
                h_type_change.bind(raw_entry->p_type.on_change);
            } else if (auto entry = dynamic_cast<nsf::entry *>(asset)) {
                h_eid_change.bind(entry->p_eid.on_change);
            }
        };
        m_tracker.on_lose <<= [this]{
            update_node_text(nullptr);

            if (h_eid_change.is_bound()) {
                h_eid_change.unbind();
                if (h_type_change.is_bound()) {
                    h_type_change.unbind();
                }
            }
        };
    }

    // (func) set_name
    // Sets the asset name this controller is assigned to.
    void set_name(res::atom name)
    {
        m_tracker.set_name(name);
        if (m_node.is_selected()) {
            m_tree.on_select(m_tracker.get_name());
        }
    }
};


/*
 * edit::mode_classic::page_ctlr
 *
 * FIXME explain
 */
class page_ctlr : private util::nocopy {
private:
    // (var) m_index
    // The index of this controller within its parent. For example, if this is
    // `6', then this controller represents the 7th page (`pages[6]') in the
    // NSF.
    int m_index;

    // (var) m_tree
    // A reference to the classic mode tree containing this controller's node.
    tree &m_tree;

    // (var) m_tracker
    // Tracks the asset name assigned to this controller.
    res::tracker<res::asset> m_tracker;

    // (var) m_node
    // The tree node associated with this controller.
    gui::treeview::node m_node;

    // (var) m_entry_ctlrs
    // The child entry controllers, if any. When this controller's name refers
    // to a standard page, this list will match the "pagelets" list from the
    // page asset. In other cases, this list will be empty.
    std::vector<std::unique_ptr<entry_ctlr>> m_entry_ctlrs;

    // (handler) h_pagelets_change
    // Watches the attached asset, if it is a standard page (spage), to update
    // the entry controllers list when the page's "pagelets" list changes.
    decltype(decltype(nsf::spage::p_pagelets)::on_change)::watch
        h_pagelets_change;

    // (handler) h_type_change
    // Watches the attached asset, if it is a standard page (spage), to update
    // the treenode text when the page's type number changes.
    decltype(decltype(nsf::spage::p_type)::on_change)::watch h_type_change;

    // (func) update_node_text
    // Sets the text for the tree node based on the state of the asset.
    void update_node_text(res::asset *asset)
    {
        if (!asset) {
            m_node.set_text("(page #$) Nothing"_fmt(m_index));
        } else if (auto spage = dynamic_cast<nsf::spage *>(asset)) {
            m_node.set_text("(page #$) Standard (T$)"_fmt(
                m_index,
                spage->get_type()
            ));
        } else if (dynamic_cast<misc::raw_data *>(asset)) {
            m_node.set_text("(page #$) Raw Data"_fmt(m_index));
        } else {
            m_node.set_text("(page #$) Incompatible Asset"_fmt(m_index));
        }
    }

public:
    // (explicit ctor)
    // FIXME explain
    explicit page_ctlr(tree &tree, gui::treeview::node &parent, int index) :
        m_index(index),
        m_tree(tree),
        m_node(parent)
    {
        update_node_text(nullptr);

        // Hook treenode select/deselect events to pass the selected asset on
        // to the owner of the tree.
        m_node.on_select <<= [this]{
            m_tree.on_select(m_tracker.get_name());
        };
        m_node.on_deselect <<= [this]{
            m_tree.on_select(nullptr);
        };

        h_pagelets_change <<= [this]{
            auto &&pagelets = static_cast<nsf::spage *>(
                m_tracker.get_name().get()
            )->get_pagelets();

            // Adjust the entry controller list to match the new number of
            // pagelets (if it has changed).
            if (m_entry_ctlrs.size() > pagelets.size()) {
                m_entry_ctlrs.resize(pagelets.size());
            } else while (m_entry_ctlrs.size() < pagelets.size()) {
                m_entry_ctlrs.push_back(std::make_unique<entry_ctlr>(
                    m_tree,
                    m_node,
                    m_entry_ctlrs.size()
                ));
            }

            // Apply the new pagelet references to the entry controllers.
            for (auto i : util::range_of(m_entry_ctlrs)) {
                m_entry_ctlrs[i]->set_name(pagelets[i]);
            }
        };

        h_type_change <<= [this]{
            update_node_text(static_cast<nsf::spage *>(
                m_tracker.get_name().get()
            ));
        };

        m_tracker.on_acquire <<= [this](res::asset *asset) {
            update_node_text(asset);

            auto spage = dynamic_cast<nsf::spage *>(asset);
            if (spage) {
                h_pagelets_change.bind(spage->p_pagelets.on_change);
                h_pagelets_change();
                h_type_change.bind(spage->p_type.on_change);
            }
        };
        m_tracker.on_lose <<= [this]{
            update_node_text(nullptr);

            if (h_pagelets_change.is_bound()) {
                h_pagelets_change.unbind();
                h_type_change.unbind();
                m_entry_ctlrs.clear();
            }
        };
    }

    // (func) set_name
    // Sets the asset name this controller is assigned to.
    void set_name(res::atom name)
    {
        m_tracker.set_name(name);
        if (m_node.is_selected()) {
            m_tree.on_select(m_tracker.get_name());
        }
    }
};

/*
 * edit::mode_classic::nsf_ctlr
 *
 * FIXME explain
 */
class nsf_ctlr : private util::nocopy {
private:
    // (var) m_tree
    // A reference to the classic mode tree containing this controller's node.
    tree &m_tree;

    // (var) m_tracker
    // Tracks the asset name assigned to this controller.
    res::tracker<res::asset> m_tracker;

    // (var) m_node
    // The tree node associated with this controller.
    gui::treeview::node m_node;

    // (var) m_page_ctlrs
    // The child page controllers, if any. When this controller's name refers
    // to an NSF archive, this list will match the archive's "pages" list. In
    // other cases, this list will be empty.
    std::vector<std::unique_ptr<page_ctlr>> m_page_ctlrs;

    // (handler) h_pages_change
    // Watches the attached asset, if it is an NSF archive, to update the page
    // controllers list when the archive's "pages" list changes.
    decltype(decltype(nsf::archive::p_pages)::on_change)::watch h_pages_change;

public:
    // (explicit ctor)
    // FIXME explain
    explicit nsf_ctlr(tree &tree, gui::treeview &parent) :
        m_tree(tree),
        m_node(parent)
    {
        m_node.set_text("NSF");

        // Hook treenode select/deselect events to pass the selected asset on
        // to the owner of the tree.
        m_node.on_select <<= [this]{
            m_tree.on_select(m_tracker.get_name());
        };
        m_node.on_deselect <<= [this]{
            m_tree.on_select(nullptr);
        };

        h_pages_change <<= [this]{
            auto &&pages = static_cast<nsf::archive *>(
                m_tracker.get_name().get()
            )->get_pages();

            // Adjust the page controller list to match the new number of pages
            // (if it has changed).
            if (m_page_ctlrs.size() > pages.size()) {
                m_page_ctlrs.resize(pages.size());
            } else while (m_page_ctlrs.size() < pages.size()) {
                m_page_ctlrs.push_back(std::make_unique<page_ctlr>(
                    m_tree,
                    m_node,
                    m_page_ctlrs.size()
                ));
            }

            // Apply the new page references to the page controllers.
            for (auto i : util::range_of(m_page_ctlrs)) {
                m_page_ctlrs[i]->set_name(pages[i]);
            }
        };

        m_tracker.on_acquire <<= [this](res::asset *asset) {
            auto nsf = dynamic_cast<nsf::archive *>(asset);
            if (nsf) {
                h_pages_change.bind(nsf->p_pages.on_change);
                h_pages_change();
            }
        };
        m_tracker.on_lose <<= [this]{
            if (h_pages_change.is_bound()) {
                h_pages_change.unbind();
                m_page_ctlrs.clear();
            }
        };
    }

    // (func) set_name
    // Sets the asset name this controller is assigned to.
    void set_name(res::atom name)
    {
        m_tracker.set_name(name);
        if (m_node.is_selected()) {
            m_tree.on_select(m_tracker.get_name());
        }
    }
};

// (inner class) impl
// Implementation class for tree (PIMPL).
class tree::impl : private util::nocopy {
    friend class tree;

private:
    // (var) m_tree
    // A reference to the outer object this implementation object is built
    // against.
    gui::treeview &m_tree;

    // (var) m_nsf_ctlr;
    // The controller for the "NSF" node in the tree.
    std::unique_ptr<nsf_ctlr> m_nsf_ctlr;

public:
    // (explicit ctor)
    // FIXME explain
    explicit impl(gui::treeview &tree) :
        m_tree(tree) {}
};

// declared in edit.hh
tree::tree(gui::container &parent, gui::layout layout) :
    treeview(parent, layout)
{
    M = new impl(*this);
}

// declared in edit.hh
tree::~tree()
{
    delete M;
}

// declared in edit.hh
void tree::set_proj(res::project *proj)
{
    if (proj) {
        M->m_nsf_ctlr = std::make_unique<nsf_ctlr>(*this, M->m_tree);
        M->m_nsf_ctlr->set_name(proj->get_asset_root() / "nsfile");
    } else {
        M->m_nsf_ctlr = nullptr;
    }
}

}
}
}
