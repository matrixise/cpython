import sys
import unittest

import dublin

functional = True

not_implemented = unittest.skip('not implemented')

# @unittest.skipIf(functional, "functional")
class DublinTests(unittest.TestCase):
    def test_doc(self):
        self.assertEqual(dublin.__doc__, "Dublin Module")

    def test_types(self):
        self.assertIn("Whisky", dublin.__dict__)

# @unittest.skipIf(functional, "functional")
class WhiskyTests(unittest.TestCase):
    def test_doc(self):
        self.assertEqual(dublin.Whisky.__doc__, "Whisky object")

    def test_constructor(self):
        whisky = dublin.Whisky()
        self.assertIsNotNone(whisky.uuid)
        self.assertEqual(2, sys.getrefcount(whisky.uuid))
        self.assertTrue(whisky.uuid.startswith("UUID: "))
        self.assertIsNone(whisky.name)

    def test_constructor_with_name(self):
        whisky = dublin.Whisky(name="Teeling")

    def test_constructor_typing_name(self):
        with self.assertRaises(TypeError):
            dublin.Whisky(name=123456789)

    def test_uuid_is_readonly(self):
        whisky = dublin.Whisky()
        with self.assertRaisesRegex(AttributeError, "readonly attribute"):
            whisky.uuid = "overriding uuid value"

    def test_access_to_name_attribute(self):
        whisky = dublin.Whisky(name="Teeling")
        self.assertEqual(whisky.name, "Teeling")

    def test_access_write_to_name_attribute(self):
        whisky = dublin.Whisky()
        self.assertIsNone(whisky.name)
        whisky.name = "Teeling"
        self.assertEqual(whisky.name, "Teeling")

    def test_has_classmethod_from_tuple(self):
        self.assertIn("from_tuple", dublin.Whisky.__dict__)
        self.assertEqual(dublin.Whisky.from_tuple.__doc__, "Convert a tuple to a Whisky")

    # @unittest.skip('not yet implemented')
    def test_call_classmethod_from_tuple(self):
        whisky = dublin.Whisky.from_tuple(('Teeling',))
        assert isinstance(whisky, dublin.Whisky)
        self.assertEqual(whisky.name, 'Teeling')

    def test_new_whisky(self):
        whisky = dublin.new_whisky()
        self.assertTrue(str(whisky).startswith("<Whisky uuid='UUID:"))
        self.assertEqual(whisky.name, "Teeling")


class TestWithSQLITE(unittest.TestCase):
    def setUp(self):
        import sqlite3
        self.conn = sqlite3.connect(':memory:')
        cur = self.conn.cursor()
        cur.execute("CREATE TABLE whiskies (name text)")
        cur.executemany("INSERT INTO whiskies VALUES (?)", [('Teeling',), ('Jameson',)])
        self.conn.commit()
        cur.close()

    def tearDown(self):
        self.conn.close()

    def test_sqlite(self):
        cur = self.conn.cursor()
        cur.execute('SELECT name FROM whiskies')
        whiskies = []
        for row in cur.fetchall():
            whiskies.append(dublin.Whisky.from_tuple(row))
        self.assertEqual(len(whiskies), 2)
        self.assertEqual(whiskies[0].name, 'Teeling')
        self.assertEqual(whiskies[1].name, 'Jameson')
        cur.close()