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
        self.assertIn("Whiskey", dublin.__dict__)

# @unittest.skipIf(functional, "functional")
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
        self.assertEqual(dublin.Whiskey.from_tuple.__doc__, "Convert a tuple to a Whiskey")

    def test_call_classmethod_from_tuple(self):
        whiskey = dublin.Whiskey.from_tuple(('Teeling',))
        assert isinstance(whiskey, dublin.Whiskey)
        self.assertEqual(whiskey.name, 'Teeling')

class TestModuleFunction(unittest.TestCase):
    def test_new_whisky(self):
        whiskey = dublin.new_whiskey()
        self.assertTrue(str(whiskey).startswith("<Whiskey uuid='UUID:"))
        self.assertEqual(whiskey.name, "Teeling")



class TestWebsiteOfWhisky(unittest.TestCase):
    def test_website(self):
        whiskey = dublin.Whiskey(name='Teeling')

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
            whiskies.append(dublin.Whiskey.from_tuple(row))
        self.assertEqual(len(whiskies), 2)
        self.assertEqual(whiskies[0].name, 'Teeling')
        self.assertEqual(whiskies[1].name, 'Jameson')
        cur.close()

class TestWithCSV(unittest.TestCase):
    def setUp(self):
        import csv
        import io
        self.strio = io.StringIO()
        writer = csv.writer(self.strio)
        writer.writerow(['Teeling'])
        writer.writerow(['Jameson'])
        self.strio.seek(0)

    def tearDown(self):
        self.strio.close()

    def test_from_tuple(self):
        import csv
        whiskies = []
        for row in csv.reader(self.strio):
            whiskies.append(dublin.Whiskey.from_tuple(tuple(row)))
        self.assertEqual(len(whiskies), 2)
        self.assertEqual(whiskies[0].name, 'Teeling')
        self.assertEqual(whiskies[1].name, 'Jameson')
