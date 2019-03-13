import sys
import unittest

import dublin

bypass = unittest.skip("by_pass")


class DublinTests(unittest.TestCase):
    def test_doc(self):
        self.assertTrue(dublin.__doc__.startswith("Dublin Module"))

    def test_types(self):
        self.assertIn("Whiskey", dublin.__dict__)

    def test_constant_version(self):
        self.assertIn("version", dublin.__dict__)
        self.assertIsInstance(dublin.version, str)
        self.assertEqual(dublin.version, "0.1")

    def test_constant_author(self):
        self.assertIn("author", dublin.__dict__)
        self.assertIsInstance(dublin.author, str)
        self.assertEqual(dublin.author, "Stéphane Wirtel")


class TestConstructor(unittest.TestCase):
    def test_doc(self):
        self.assertEqual(dublin.Whiskey.__doc__, "Whiskey object")

    def test_constructor(self):
        whiskey = dublin.Whiskey()
        self.assertIsNotNone(whiskey.uuid)
        self.assertEqual(2, sys.getrefcount(whiskey.uuid))
        self.assertTrue(whiskey.uuid.startswith("UUID: "))
        self.assertIsNone(whiskey.name)

    def test_constructor_with_name(self):
        whiskey = dublin.Whiskey(name="Teeling")

    def test_constructor_typing_name(self):
        with self.assertRaises(TypeError):
            dublin.Whiskey(name=123456789)


class TestAttributes(unittest.TestCase):
    def test_uuid_is_readonly(self):
        whiskey = dublin.Whiskey()
        with self.assertRaisesRegex(AttributeError, "readonly attribute"):
            whiskey.uuid = "overriding uuid value"

    def test_access_to_name_attribute(self):
        whiskey = dublin.Whiskey(name="Teeling")
        self.assertEqual(whiskey.name, "Teeling")

    def test_access_write_to_name_attribute(self):
        whiskey = dublin.Whiskey()
        self.assertIsNone(whiskey.name)
        whiskey.name = "Teeling"
        self.assertEqual(whiskey.name, "Teeling")


class TestClassMethod(unittest.TestCase):
    def test_has_classmethod_from_tuple(self):
        self.assertIn("from_tuple", dublin.Whiskey.__dict__)
        self.assertTrue(
            dublin.Whiskey.from_tuple.__doc__.startswith("Whiskey.from_tuple")
        )

    def test_call_classmethod_from_tuple(self):
        whiskey = dublin.Whiskey.from_tuple(("Teeling",))
        assert isinstance(whiskey, dublin.Whiskey)
        self.assertEqual(whiskey.name, "Teeling")


class TestModuleFunction(unittest.TestCase):
    def test_new_whisky(self):
        whiskey = dublin.new_whiskey()
        self.assertTrue(str(whiskey).startswith("<Whiskey uuid='UUID:"))
        self.assertEqual(whiskey.name, "Teeling")


class TestWithSQLITE(unittest.TestCase):
    def setUp(self):
        import sqlite3

        self.conn = sqlite3.connect(":memory:")
        cur = self.conn.cursor()
        cur.execute("CREATE TABLE whiskies (name text)")
        cur.executemany("INSERT INTO whiskies VALUES (?)", [("Teeling",), ("Jameson",)])
        self.conn.commit()
        cur.close()

    def tearDown(self):
        self.conn.close()

    def test_sqlite(self):
        cur = self.conn.cursor()
        cur.execute("SELECT name FROM whiskies")
        whiskies = []
        for row in cur.fetchall():
            whiskies.append(dublin.Whiskey.from_tuple(row))
        self.assertEqual(len(whiskies), 2)
        self.assertEqual(whiskies[0].name, "Teeling")
        self.assertEqual(whiskies[1].name, "Jameson")
        cur.close()


class TestWithCSV(unittest.TestCase):
    def setUp(self):
        import csv
        import io

        self.strio = io.StringIO()
        writer = csv.writer(self.strio)
        writer.writerow(["Teeling"])
        writer.writerow(["Jameson"])
        self.strio.seek(0)

    def tearDown(self):
        self.strio.close()

    def test_from_tuple(self):
        import csv

        whiskies = []
        for row in csv.reader(self.strio):
            whiskies.append(dublin.Whiskey.from_tuple(tuple(row)))
        self.assertEqual(len(whiskies), 2)
        self.assertEqual(whiskies[0].name, "Teeling")
        self.assertEqual(whiskies[1].name, "Jameson")


class TestPyCons(unittest.TestCase):
    def test(self):
        pycons = dublin.pycons()
        self.assertIsInstance(pycons, list)
        self.assertEqual(len(pycons), 1)
        pycon = pycons[0]
        self.assertIsInstance(pycon, dublin.PyCon)
        with self.assertRaisesRegex(AttributeError, "readonly attribute"):
            pycon.start_on = 42


class TestLoadPyCons(unittest.TestCase):
    def test_reject_type_other_string(self):
        with self.assertRaises(TypeError):
            dublin.load_pycons(1)

    def test_can_read_database(self):
        pycons = dublin.load_pycons("/tmp/irish.db")
        self.assertIsNotNone(pycons)
        self.assertEqual(len(pycons), 2)
        self.assertTrue(all(isinstance(instance, dublin.PyCon) for instance in pycons))

    def test_cannot_read_database(self):
        import sqlite3
        with self.assertRaises(sqlite3.OperationalError):
            pycons = dublin.load_pycons("/tmp/ireland.db")

class DublinDummyFunctionTests(unittest.TestCase):
    def test_has_dummy_function(self):
        self.assertIn('dummy_function', dublin.__dict__)

    def test_raise_exception(self):
        with self.assertRaises(NotImplementedError):
            dublin.dummy_function()
