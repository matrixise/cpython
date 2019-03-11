import unittest

import dublin


class DublinTests(unittest.TestCase):
    def test_doc(self):
        self.assertEqual(dublin.__doc__, "Dublin Module")

    def test_types(self):
        self.assertIn("Whisky", dublin.__dict__)


class WhiskyTests(unittest.TestCase):
    def test_doc(self):
        self.assertEqual(dublin.Whisky.__doc__, "Whisky object")

    def test_constructor(self):
        whisky = dublin.Whisky()
        self.assertIsNotNone(whisky.uuid)
        self.assertTrue(whisky.uuid.startswith("UUID: Whisky:"))
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
        with self.assertRaises(NotImplementedError):
            dublin.Whisky.from_tuple()
