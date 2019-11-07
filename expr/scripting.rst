This is an experimental documentation file for the DRNSF scripting API. It may
be completely rewritten, reformatted, or scrapped in the future.

TBD

Legend
------

*(read-only)*
  * For attributes: the attribute cannot be set.


Atoms
=====

TBD

Atom objects may be compared using ``==`` or ``!=``. Avoid using ``is`` or
``is not``. Most built-in functions and attributes return a new Atom object at
each invocation or reference.

Atom objects support path building using the "true division" operator ``/``.
For example, given a project ``p``, both ``Atom('/A/B/C', p)`` and
``p.root / 'A' / 'B' / 'C'`` are equivalent.

.. py:class:: Atom(path, project)

   * *path* :py:class:`str`
   * *project* :py:class:`Project`

   Constructs an atom referring to the node at *path* within *project*. If one
   does not exist, it is created. If *path* is an empty string, constructs an
   atom referring to the root node of *project*.

   .. py:attribute:: parent

      *(read-only)* :py:class:`Atom` or ``None``

      An atom referring to this atom's node's parent node.

      For example, the node ``/A/B/C/D`` has parent node ``/A/B/C``, so
      ``Atom('/A/B/C/D', p).parent == Atom('/A/B/C', p)``.

      For a root node (:py:attr:`Project.root`), this is always ``None``.

   .. py:attribute:: basename

      *(read-only)* :py:class:`str`

      The name of the node referred to by this atom, excluding the path of its
      parent. For example, an atom referring to the node ``/A/B/C/D`` has
      basename ``'D'``.

      For a root node (:py:attr:`Project.root`), this is an empty string.

   .. py:attribute:: dirname

      *(read-only)* :py:class:`str`

      The path of this atom's node's parent. For example, an atom referring to
      the node ``/A/B/C/D`` has dirname ``'/A/B/C'``.

      For a root node (:py:attr:`Project.root`), this is an empty string.

   .. py:attribute:: path

      *(read-only)* :py:class:`str`

      The path of this atom's node. For example, an atom referring to the node
      ``/A/B/C/D`` has path ``'/A/B/C/D'``.

      For a root node (:py:attr:`Project.root`), this is an empty string.

   .. py:function:: firstchild()

      Returns :py:class:`Atom` or ``None``

      Returns an atom referring to the first child node of this atom's node, or
      ``None`` if there are no children.

   .. py:function:: nextsibling()

      Returns :py:class:`Atom` or ``None``

      Returns an atom referring to the next sibling node of this atom's node;
      that is, the next node which shares the same parent. Returns ``None`` if
      this atom's node is the last or only child of its parent node.

      For a root node (:py:attr:`Project.root`), this always returns ``None``.


Projects
========

TBD

.. py:class:: Project()

   Constructs a new empty project.

   .. py:attribute:: root

      *(read-only)* :py:class:`Atom`

      The root atom of the project. All assets in a project have a name that is
      a descendant of the project's root atom.

      The root atom cannot be the name of an asset.

.. py:function:: getcontextproject()

   Returns :py:class:`Project` or ``None``

   Returns the project which is currently open in the user interface. Returns
   ``None`` if there is no open project.
